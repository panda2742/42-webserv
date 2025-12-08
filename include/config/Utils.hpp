#ifndef PARSE_UTILS_HPP
#define PARSE_UTILS_HPP

#include "Parser.hpp"

namespace cfg
{
// #########################################################

namespace Utils
{

bool						isInCharset(char c, const char *cstr);
void						printTokens_(const std::vector<Lexer::TokenNode>& nodes);
std::vector<std::string>	&cleanVector(std::vector<std::string>& vect);
bool						isNumber(const std::string& nbstr);
std::string					vecStr(const std::vector<std::string>& v);
std::string					vecUIntStr(const std::vector<unsigned int>& v);
std::string					mapStr(const std::map<unsigned int, std::string>& m);
std::string					mapVecStr(const std::map<unsigned int, std::vector<std::string> >& m);

};

// #########################################################
};

#endif