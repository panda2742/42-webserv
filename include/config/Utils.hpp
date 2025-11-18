#ifndef PARSE_UTILS_HPP
#define PARSE_UTILS_HPP

#include "Parser.hpp"

namespace Config
{
namespace Utils
{
// #########################################################

bool						isInCharset(char c, const char *cstr);
void						printTokens_(const std::vector<Lexer::TokenNode>& nodes);
std::vector<std::string>	&cleanVector(std::vector<std::string>& vect);

// #########################################################
};
};

#endif