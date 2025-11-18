#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

namespace Config
{
namespace Lexer
{
// #########################################################

enum Token
{
	TokenSymbolOpen,
	TokenSymbolClose,
	TokenDelimiter,
	TokenDirective,
	TokenParent,
	TokenArgument
};

struct TokenNode
{
	Token				type;
	const std::string	value;
	TokenNode(const Token type, const std::string& value)
		: type(type), value(value) {}
};

// #########################################################
};
};

#endif