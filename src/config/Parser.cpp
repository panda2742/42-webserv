#include "config/Parser.hpp"
#include "config/Utils.hpp"
#include <fstream>
#include <iostream>

namespace Config
{
// #########################################################

ParsingException::ParsingException(const std::string& msg)
	: std::runtime_error(RED PARSING_ERROR_MSG + msg + RESET) {}

Parser::Parser(const std::string& filename)
	: config_(), filename_(filename) {}

Parser::~Parser(void) {}

void	Parser::saveRaw_(void) throw(ParsingException)
{
	std::ifstream	file(filename_.c_str(), std::ios::in | std::ios::binary);
	if (!file)
		throw ParsingException("file is not openable.");

	file.seekg(0, std::ios::end);
	std::ifstream::pos_type size = file.tellg();
	file.seekg(0, std::ios::beg);

	raw_config_.resize(static_cast<std::string::size_type>(size));

	if (size > 0)
		file.read(&raw_config_[0], size);
}

void	Parser::tokenize_(void) throw(ParsingException)
{
	const char	*raw = raw_config_.c_str();
	size_t	i = 0;
	std::vector<std::string>	shards;
	while (raw[i])
	{
		size_t	j = i;

		while (raw[j])
		{
			if (Utils::isInCharset(raw[j], " \n\t\r\v"))
			{
				std::string	value(&raw[i], j - i);
				if (value.length())
					shards.push_back(value);
				break;
			}
			j++;
		}
		i = j + 1;
	}
	shards = Utils::cleanVector(shards);
	for (std::vector<std::string>::const_iterator it = shards.begin(); it != shards.end(); ++it)
	{
		Lexer::Token	type = Lexer::TokenDirective;
		if (!lexer_nodes_.empty())
		{
			if ((*it) == "{")
				type = Lexer::TokenSymbolOpen;
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

HttpConfig&	Parser::parse(void) throw(ParsingException)
{
	saveRaw_();
	tokenize_();
	Utils::printTokens_(lexer_nodes_);

	return config_;
}

// #########################################################
};