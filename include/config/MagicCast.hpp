#ifndef PARSE_UTILS_HPP
#define PARSE_UTILS_HPP

#include "Parser.hpp"

#define INVALID_CAST_EXCEPTION "the value cannot be converted from a string to an unsigned int, to prevent data loss."

namespace Config
{
// #########################################################

class MagicCastException: public std::runtime_error
{
	public:
	MagicCastException(const std::string& err_msg);
};

class InvalidCastException: public MagicCastException
{
	public:
	InvalidCastException(void);
};

/**
 * Convert a type to another. This is pure sorcery. There is the available types:
 * - string
 * - unsigned int
 * - vector<string>
 * - vector<unsigned int>
 * - map<unsigned int, string>
 * - map<unsigned int, vector<string>>
 *
 * Even if this function is pure sorcery, conversions are sometimes not even possible. In this case, an error will be
 * thrown. And it is just when we try to convert an unsigned int to a string, no matter the type.
 */
template <typename R, typename T>
R	magic_cast(T value) throw(MagicCastException);

template <typename A, typename B>
A&	magic_refcpy(A& a, B& b) throw(MagicCastException);

template <typename A, typename B>
A	magic_cpy(A& a, B& b) throw(MagicCastException);

// #########################################################
};

#include "MagicCast.tpp"

#endif