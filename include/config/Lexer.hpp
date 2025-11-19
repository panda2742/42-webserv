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
	TokenNode(const Token type_, const std::string& value_)
		: type(type_), value(value_) {}
};

// #########################################################
};
};

#endif