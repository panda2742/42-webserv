#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <ostream>
#include "types.hpp"

namespace cfg
{
namespace Lexer
{
// #########################################################

/**
 * The different types of token for the lexer.
 */
enum Token
{
	TokenSymbolOpen,
	TokenSymbolClose,
	TokenDelimiter,
	TokenDirective,
	TokenParent,
	TokenArgument
};

/**
 * Represent a node for the lexer.
 */
struct TokenNode
{
	/**
	 * The type of the token.
	 */
	Token		type;

	/**
	 * The text value of the token, extracted from the configuration file.
	 */
	std::string	value;

	TokenNode(const Token type_, const std::string& value_)
		: type(type_), value(value_) {}
};

std::ostream&	operator<<(std::ostream& os, const TokenNode& node);

// #########################################################
};
};

#endif