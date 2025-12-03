#ifndef PARSE_UTILS_HPP
#define PARSE_UTILS_HPP

#include "Parser.hpp"

#define INVALID_STRING_EXCEPTION "a string can only be casted to: vector<string>"
#define INVALID_VECTOR_STRING_EXCEPTION "a vector<string> can only be casted to: string"
#define INVALID_VECTOR_UINT_EXCEPTION "a vector<unsigned int> can only be casted to: string, vector<string>, map<unsigned int, string>, map<unsigned int, vector<string>>"
#define INVALID_MAP_STRING_EXCEPTION "a map<unsigned int, string> can only be casted to: string, vector<string>, map<unsigned int, vector<string>>"
#define INVALID_MAP_VECTOR_STRING_EXCEPTION "a map<unsigned int, vector<string>> can only be casted to: string, vector<string>, map<unsigned int, string>"

namespace Config
{
// #########################################################

class MagicCastException: public std::runtime_error
{
	public:
	MagicCastException(const std::string& err_msg);
};

class InvalidStringException: public MagicCastException
{
	public:
	InvalidStringException(void);
};

class InvalidVectorStringException: public MagicCastException
{
	public:
	InvalidVectorStringException(void);
};

class InvalidVectorUintException: public MagicCastException
{
	public:
	InvalidVectorUintException(void);
};

class InvalidMapStringException: public MagicCastException
{
	public:
	InvalidMapStringException(void);
};

class InvalidMapVectorStringException: public MagicCastException
{
	public:
	InvalidMapVectorStringException(void);
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