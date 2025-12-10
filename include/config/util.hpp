#ifndef PARSE_UTILS_HPP
#define PARSE_UTILS_HPP

#include "Parser.hpp"

namespace cfg
{
// #########################################################

namespace util
{

/**
 * Test if a character is present in a set of chars.
 *
 * @param c The character to test.
 * @param cstr The char set.
 * @return A boolean that is the result of the test.
 */
bool						isInCharset(char c, const char *cstr);

/**
 * Display the lexer nodes passed as parameter.
 *
 * @param nodes The sequence of nodes to display.
 */
void						printTokens_(const std::vector<Lexer::TokenNode>& nodes);

/**
 *
 */
std::vector<std::string>	&cleanVector(std::vector<std::string>& vect);

/**
 * Test if a string is a number.
 *
 * @param nbstr The string to test.
 * @return The result of the test as a boolean.
 */
bool						isNumber(const std::string& nbstr);

/**
 * Return a string containing an ANSI escape color code associated with the type of the value.
 *
 * @param value The value to colorize.
 * @return The ANSI escape color code associated with the type.
 */
template <typename T>
std::string	colorize(T& value);

/**
 * Create a string based on each strings of the parameter. Every string is assembled with a space.
 *
 * @param v The vector of string to concatenate.
 * @return The built string.
 */
std::string					vecStr(const std::vector<std::string>& v);

/**
 * Create a string on each unsigned int of the parameter. Every unsinged int is assembled with a space.
 *
 * @param v The vector of unsigned int to concatenate.
 * @return The built string.
 */
std::string					vecUIntStr(const std::vector<unsigned int>& v);

/**
 * Create a string with the keys of the map and the values of the map. Every key-value is assembled with a space.
 *
 * @param m The map to concatenate.
 * @return The built string.
 */
std::string					mapStr(const std::map<unsigned int, std::string>& m);

/**
 * Create a string with the keys of the map and the values of the map. Every key-value is assembled with a space.
 *
 * @param m The map to concatenate.
 * @return The built string.
 */
std::string					mapVecStr(const std::map<unsigned int, std::vector<std::string> >& m);

};

// #########################################################
};

#endif