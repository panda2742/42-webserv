#include "config/Lexer.hpp"

namespace Config
{
namespace Lexer
{
// #########################################################

TokenNode::TokenNode(const Token type, const std::string& value)
	: type(type), value(value) {}

// #########################################################
};
};
