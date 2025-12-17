#include "config/Parser.hpp"
#include "config/util.hpp"
#include <fstream>
#include <iostream>

namespace cfg
{
// #########################################################

void	Parser::saveRaw_(void) throw(ParsingException)
{
	std::ifstream	file(filename_.c_str(), std::ios::in | std::ios::binary);
	if (!file)
		throw ParsingException("file is not openable.");

	file.seekg(0, std::ios::end);
	std::ifstream::pos_type size = file.tellg();
	file.seekg(0, std::ios::beg);

	raw_config_.resize(static_cast<str_t::size_type>(size));

	if (size > 0)
		file.read(&raw_config_[0], size);

	raw_config_ += "\n";

	bool		in_comment = false;
	str_t	result;
    for (str_t::size_type i = 0; i < raw_config_.length(); ++i) {
        if (in_comment)
		{
            if (raw_config_[i] == '\n')
			{
                in_comment = false;
                result += '\n';
            }
        }
		else
		{
            if (raw_config_[i] == '#')
                in_comment = true;
            else
                result += raw_config_[i];
        }
    }

	raw_config_ = result;
}

void	Parser::tokenize_(void) throw(ParsingException)
{
	const char	*raw = raw_config_.c_str();
	size_t	i = 0,
			raw_length = raw_config_.length();
	vecstr_t	shards;
	while (i < raw_length)
	{
		size_t	j = i;

		while (j < raw_length)
		{
			if (util::isInCharset(raw[j], " \n\t\r\v"))
			{
				str_t	value(&raw[i], j - i);
				if (value.length())
					shards.push_back(value);
				break;
			}
			j++;
		}
		i = j + 1;
	}
	shards = util::cleanVector(shards);
	for (vecstr_t::const_iterator it = shards.begin(); it != shards.end(); ++it)
	{
		Lexer::Token	type = Lexer::TokenDirective;
		if (!lexer_nodes_.empty())
		{
			if ((*it) == "{")
			{
				type = Lexer::TokenSymbolOpen;
				for (std::vector< cfg::Lexer::TokenNode>::reverse_iterator jt = lexer_nodes_.rbegin(); jt != lexer_nodes_.rend(); ++jt)
				{
					if ((*jt).type == Lexer::TokenDirective)
					{
						(*jt).type = Lexer::TokenParent;
						break;
					}
				}
			}
			else if ((*it) == "}")
				type = Lexer::TokenSymbolClose;
			else if ((*it) == ";")
				type = Lexer::TokenDelimiter;
			else if (lexer_nodes_.back().type == Lexer::TokenDirective || lexer_nodes_.back().type == Lexer::TokenArgument)
				type = Lexer::TokenArgument;
		}
		lexer_nodes_.push_back(Lexer::TokenNode(type, (*it)));
	}
}

void	Parser::parse(void) throw(ParsingException)
{
	saveRaw_();
	tokenize_();
	// util::printTokens_(lexer_nodes_);
}

const std::vector<Lexer::TokenNode>&	Parser::getNodes(void) const{
	return lexer_nodes_;
}

// #########################################################
};