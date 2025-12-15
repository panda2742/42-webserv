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
 * @tparam T The type of the value to get the color from.
 * @return The ANSI escape color code associated with the type.
 */
template <typename T>
std::string					colorize(const T&);

/**
 * Take a variable of type T, where T must be a valid node type. It displays correctly the asked value with colors
 * based on types of each member of the value.
 *
 * @tparam T The type of the value.
 * @param value The value to represent.
 * @return The built string representation of the value.
 */
template <typename T>
std::string					represent(const T& value);

};

// #########################################################
};

#include "util.tpp"

#endif