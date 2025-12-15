#include "Parser.hpp"
#include "MagicCast.hpp"
#include <typeinfo>
#include <sstream>

namespace cfg
{
// #########################################################

namespace util
{

template <typename T>
inline std::string	colorize(const T&)
{
	return RESET;
}

template <>
inline std::string	colorize<std::string>(const std::string&)
{
	return ORANGE;
}

template <>
inline std::string	colorize<unsigned int>(const unsigned int&)
{
	return LIGHT_GREEN;
}

template <>
inline std::string	colorize<std::vector<std::string> >(const std::vector<std::string>&)
{
	return GREEN;
}

template <>
inline std::string	colorize<std::vector<unsigned int> >(const std::vector<unsigned int>&)
{
	return CYAN;
}

template <>
inline std::string	colorize<std::map<unsigned int, std::string> >(const std::map<unsigned int, std::string>&)
{
	return BLURPLE;
}

template <>
inline std::string	colorize<std::map<unsigned int, std::vector<std::string> > >(const std::map<unsigned int, std::vector<std::string> > &)
{
	return PINK;
}

template <typename T>
std::string	represent(const T& value)
{
	const std::type_info&	value_type = typeid(value);
	const std::string		main_color = colorize(value);
	std::stringstream		s;

	if (value_type == typeid(std::string))
	{
		const std::string	v = magic_cast<std::string>(value);
		s << main_color << v << RESET;
	}
	else if (value_type == typeid(unsigned int))
	{
		const unsigned int	v = magic_cast<unsigned int>(value);
		s << main_color << v << RESET;
	}
	else if (value_type == typeid(std::vector<std::string>))
	{
		const std::vector<std::string>	v = magic_cast<std::vector<std::string> >(value);
		s << main_color << "[";
		for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const std::string	color = colorize(*it);
			s << color << (*it);
		}
		s << main_color << "]" << RESET;
	}
	else if (value_type == typeid(std::vector<unsigned int>))
	{
		const std::vector<unsigned int>	v = magic_cast<std::vector<unsigned int> >(value);
		s << main_color << "[";
		for (std::vector<unsigned int>::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const std::string	color = colorize(*it);
			s << color << (*it);
		}
		s << main_color << "]" << RESET;
	}
	else if (value_type == typeid(std::map<unsigned int, std::string>))
	{
		const std::map<unsigned int, std::string>	v = magic_cast<std::map<unsigned int, std::string> >(value);
		s << main_color << "{";
		for (std::map<unsigned int, std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const unsigned int	it_k = (*it).first;
			const std::string	it_v = (*it).second;
			s << colorize(it_k) << it_k << main_color << ": " << colorize(it_v) << it_v;
		}
		s << main_color << "}";
	}
	else if (value_type == typeid(std::map<unsigned int, std::vector<std::string> >))
	{
		const std::map<unsigned int, std::vector<std::string> >	v =
			magic_cast<std::map<unsigned int, std::vector<std::string> > >(value);
		s << main_color << "{";
		for (std::map<unsigned int, std::vector<std::string> >::const_iterator it = v.begin(); it != v.end(); ++it)
		{
			if (it != v.begin())
				s << ", ";
			const unsigned int				it_k = (*it).first;
			const std::vector<std::string>	it_v = (*it).second;
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
