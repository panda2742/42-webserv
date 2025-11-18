#include "config/Utils.hpp"
#include <iostream>
#include "config/Lexer.hpp"

namespace Config
{
namespace Utils
{
// #########################################################

bool	isInCharset(char c, const char *cstr)
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

void	printTokens_(const std::vector<Lexer::TokenNode>& nodes)
{
	for (std::vector<Lexer::TokenNode>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (it != nodes.begin())
			std::cout << "    ↓" << std::endl;
		std::cout << "";
		switch ((*it).type)
		{
			case Lexer::TokenSymbolOpen:
				std::cout << ORANGE "SymbolOpen" RESET;
				break;
			case Lexer::TokenSymbolClose:
				std::cout << GREEN "SymbolClose" RESET;
				break;
			case Lexer::TokenDirective:
				std::cout << CYAN "Directive" RESET;
				break;
			case Lexer::TokenDelimiter:
				std::cout << BLURPLE "Delimiter" RESET;
				break;
			case Lexer::TokenArgument:
				std::cout << PINK "Argument" RESET;
				break;
			default:
				std::cout << "UNKNOWN (" << (*it).type << ")";
				break;
		}
		std::cout << "\t" GREY << (*it).value << RESET << std::endl;
	}
}

std::vector<std::string>&	cleanVector(std::vector<std::string>& vect)
{
	std::vector<std::string>	res;

	for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); ++it)
	{
		const std::string&	str = (*it);
		std::string			current;

		for (size_t i = 0; i < str.length(); ++i)
		{
			char	c = str[i];
			if (isInCharset(c, "{};"))
			{
				if (!current.empty())
				{
					res.push_back(current);
					current.clear();
				}
				res.push_back(std::string(1, c));
			}
			else
				current += c;
		}

		if (!current.empty())
			res.push_back(current);
	}

	vect.swap(res);
	return vect;
}

// #########################################################
};
};
