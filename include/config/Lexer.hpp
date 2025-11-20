#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <ostream>
#include "types.hpp"

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

std::ostream&	operator<<(std::ostream& os, const TokenNode& node);

// #########################################################
};
};

#endif