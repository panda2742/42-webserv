#include "parsing/Parser.hpp"
#include <fstream>
#include <iostream>

namespace Parsing
{
	namespace AST
	{
		ASTNode::ASTNode(const Token type, const std::string& value)
			: type(type), value(value) {}
	}

	ParsingException::ParsingException(const std::string& msg)
		: std::runtime_error(RED PARSING_ERROR_MSG + msg + RESET) {}

	bool	Parser::isInCharset(char c, const char *cstr)
	{
		if (c == 0)
			return false;
		while (*cstr)
		{
			if (*cstr == c)
				return true;
			cstr++;
		}
		return false;
	}

	void	Parser::printTokens_(const ASTNodes& nodes)
	{
		for (ASTNodes::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			if (it != nodes.begin())
				std::cout << "    ↓" << std::endl;
			std::cout << "";
			switch ((*it).type)
			{
				case AST::TokenSymbolOpen:
					std::cout << ORANGE "SymbolOpen" RESET;
					break;
				case AST::TokenSymbolClose:
					std::cout << GREEN "SymbolClose" RESET;
					break;
				case AST::TokenDirective:
					std::cout << CYAN "Directive" RESET;
					break;
				case AST::TokenDelimiter:
					std::cout << BLURPLE "Delimiter" RESET;
					break;
				case AST::TokenArgument:
					std::cout << PINK "Argument" RESET;
					break;
				default:
					std::cout << "UNKNOWN (" << (*it).type << ")";
					break;
			}
			std::cout << "\t" GREY << (*it).value << RESET << std::endl;
		}
	}

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
		while (raw[i])
		{
			size_t	j = i;

			while (raw[j])
			{
				// std::cout << "j = " << j << ": " << raw[j] << std::endl;
				if (isInCharset(raw[j], " \t\r\n\v;"))
				{
					++j;
					continue;
				}

				const char	*str = &raw[i];
				size_t		k = 0;
				std::string	value;
				while (*str)
				{
					if (isInCharset(*str, "{};"))
					{
						if (value.length() > 0)
						{
							ast_nodes_.push_back(AST::ASTNode(AST::TokenArgument, value));
							value.clear();
						}
						std::string	del(str, 1);
						ast_nodes_.push_back(AST::ASTNode(AST::TokenDelimiter, del));
					}
					else
						value.push_back(*str);
					++str;
					++k;
				}

				i += k;

				break;
			}
			i = j + 1;
		}
	}

	WebservConf&	Parser::parse(void) throw(ParsingException)
	{
		saveRaw_();
		tokenize_();
		printTokens_(ast_nodes_);

		return config_;
	}
}