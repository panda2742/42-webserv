#ifndef MAGIC_CAST_HPP
#define MAGIC_CAST_HPP

#include "Parser.hpp"

#define INVALID_STRING_EXCEPTION "a string can only be casted to: vector<string>"
#define INVALID_VECTOR_STRING_EXCEPTION "a vector<string> can only be casted to: string"
#define INVALID_VECTOR_UINT_EXCEPTION "a vector<unsigned int> can only be casted to: string, vector<string>, map<unsigned int, string>, map<unsigned int, vector<string>>"
#define INVALID_MAP_STRING_EXCEPTION "a map<unsigned int, string> can only be casted to: string, vector<string>, map<unsigned int, vector<string>>"
#define INVALID_MAP_VECTOR_STRING_EXCEPTION "a map<unsigned int, vector<string>> can only be casted to: string, vector<string>, map<unsigned int, string>"

#define ASSEMBLE_TOO_PRIMITIVE "your type cannot be merged with another, it musts be a container"

namespace cfg
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

class AssembleTooPrimitive: public MagicCastException
{
	public:
	AssembleTooPrimitive(void);
};

/**
 * This is a magic casting function. This function can cast a type into another, where universe limitations do not
 * operate. You probably guess what is the purpose of this function, and you are right. So, In C++, the problem is the
 * language is too restrictive when it comes to manipulate types. The compiler was basically my ennemy, so I decided
 * to trick him and to allow weird conversions. It is used to convert a type that "fits" requesites to another to
 * universalize the logical behind the call, and to prevent wrong types by throwing exceptions on invalid casts.
 *
 * ### There is the conversions table:
 *
 * ##### `string` -> `vector<string>`
 *
 * ##### `unsigned int`  -> `string`, `vector<string>`, `vector<unsigned int>`, `map<unsigned int, string>`, `map<unsigned int, vector<string>>`
 *
 * ##### `vector<string>` -> `string`
 *
 * ##### `vector<unsigned int>`  -> `string`, `vector<string>`, `map<unsigned int, string>`, `map<unsigned int, vector<string>>`
 *
 * ##### `map<unsigned int, string>` -> `string`, `vector<string>`, `map<unsigned int, vector<string>>`
 *
 * ##### `map<unsigned int, vector<string>>` -> `string`, `vector<string>`, `map<unsigned int, string>`
 *
 * @tparam R The type to convert the value in.
 * @tparam T The current type (auto deduced) of the value to convert.
 * @param value The value to convert/cast.
 * @return A new instance of the R type, containing the value adapted as required by the server.
 */
template <typename R, typename T>
R	magic_cast(T value) throw(MagicCastException);

/**
 * Only goats can understand this shit. If a cast is invalid, an error will be thrown.
 *
 * @tparam R The type of the returned value.
 * @tparam A The type of the first element.
 * @tparam B The type of the second element.
 * @param a The first element.
 * @param b The second element.
 * @return A new instance of type R containing the assembling of the two elements.
 */
template <typename R, typename A, typename B>
R	magic_assemble(A& a, B& b) throw (MagicCastException);

std::vector<std::string>							magic_assemble_sub_(std::vector<std::string>& a, std::vector<std::string>& b) throw (MagicCastException);
std::vector<unsigned int>							magic_assemble_sub_(std::vector<unsigned int>& a, std::vector<unsigned int>& b) throw (MagicCastException);
std::map<unsigned int, std::string>					magic_assemble_sub_(std::map<unsigned int, std::string>& a, std::map<unsigned int, std::string>& b) throw (MagicCastException);
std::map<unsigned int, std::vector<std::string> >	magic_assemble_sub_(std::map<unsigned int, std::vector<std::string> >& a, std::map<unsigned int, std::vector<std::string> >& b) throw (MagicCastException);

// #########################################################
};

#include "MagicCast.tpp"

#endif