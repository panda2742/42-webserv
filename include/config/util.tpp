#include "Parser.hpp"
#include "MagicCast.hpp"
#include <typeinfo>
#include <sstream>
#include "global.hpp"

namespace cfg
{
// #########################################################

namespace util
{

template <typename T>
inline str_t	colorize(const T&)
{
	return RESET;
}

template <>
inline str_t	colorize<str_t>(const str_t&)
{
	return ORANGE;
}

template <>
inline str_t	colorize<uint_t>(const uint_t&)
{
	return LIGHT_GREEN;
}

template <>
inline str_t	colorize<vecstr_t >(const vecstr_t&)
{
	return GREEN;
}

template <>
inline str_t	colorize<vecuint_t >(const vecuint_t&)
{
	return CYAN;
}

template <>
inline str_t	colorize<mapstr_t >(const mapstr_t&)
{
	return BLURPLE;
}

template <>
inline str_t	colorize<mapvec_t>(const mapvec_t &)
{
	return PINK;
}

template <typename T>
str_t	represent(const T& value)
{
	const std::type_info&	value_type = typeid(value);
	const str_t		main_color = colorize(value);
	std::stringstream		s;

	if (value_type == typeid(str_t))
	{
		const str_t	v = magic_cast<str_t>(value);
		s << main_color << v << RESET;
	}
	else if (value_type == typeid(uint_t))
	{
		const uint_t	v = magic_cast<uint_t>(value);
		s << main_color << v << RESET;
	}
	else if (value_type == typeid(vecstr_t))
	{
		const vecstr_t	v = magic_cast<vecstr_t >(value);
		s << main_color << "[";
		for (vecstr_t::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const str_t	color = colorize(*it);
			s << color << '\'' << (*it) << '\'';
		}
		s << main_color << "]" << RESET;
	}
	else if (value_type == typeid(vecuint_t))
	{
		const vecuint_t	v = magic_cast<vecuint_t >(value);
		s << main_color << "[";
		for (vecuint_t::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const str_t	color = colorize(*it);
			s << color << (*it);
		}
		s << main_color << "]" << RESET;
	}
	else if (value_type == typeid(mapstr_t))
	{
		const mapstr_t	v = magic_cast<mapstr_t >(value);
		s << main_color << "{";
		for (mapstr_t::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const uint_t	it_k = (*it).first;
			const str_t	it_v = (*it).second;
			s << colorize(it_k) << it_k << main_color << ": " << colorize(it_v) << it_v;
		}
		s << main_color << "}";
	}
	else if (value_type == typeid(mapvec_t))
	{
		const mapvec_t	v =
			magic_cast<mapvec_t>(value);
		s << main_color << "{";
		for (mapvec_t::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const uint_t				it_k = (*it).first;
			const vecstr_t	it_v = (*it).second;
			s << colorize(it_k) << it_k << main_color << ": " << represent(it_v);
		}
		s << main_color << "}";
	}

	s << RESET;
	return s.str();
}

};

// #########################################################
};
