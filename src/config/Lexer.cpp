#include "config/Lexer.hpp"

namespace Config
{
namespace Lexer
{
// #########################################################

std::ostream&	operator<<(std::ostream& os, const TokenNode& node)
{
	switch (node.type)
	{
		case Lexer::TokenSymbolOpen:
			os << ORANGE "SymbolOpen" RESET;
			break;
		case Lexer::TokenSymbolClose:
			os << GREEN "SymbolClose" RESET;
			break;
		case Lexer::TokenDirective:
			os << CYAN "Directive" RESET;
			break;
		case Lexer::TokenParent:
			os << LIGHT_GREEN "ParentDir" RESET;
			break;
		case Lexer::TokenDelimiter:
			os << BLURPLE "Delimiter" RESET;
			break;
		case Lexer::TokenArgument:
			os << PINK "Argument" RESET;
			break;
		default:
			os << "UNKNOWN (" << node.type << ")";
			break;
	}
	os << "\t" GREY << node.value << RESET << std::endl;
	return os;
}

// #########################################################
};
};

