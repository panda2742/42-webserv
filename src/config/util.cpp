#include "config/util.hpp"
#include <iostream>
#include "config/Lexer.hpp"
#include <sstream>

namespace cfg
{
namespace util
{
// #########################################################

bool	isInCharset(char c, const char *cstr)
{
	if (c == 0)
		return false;
	while (*cstr)
	{
		if (*cstr == c)
			return true;
		cstr++;
	}
	return false;
}

void	printTokens_(const std::vector<Lexer::TokenNode>& nodes)
{
	for (std::vector<Lexer::TokenNode>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (it != nodes.begin())
			std::cout << "    ↓" << std::endl;
		std::cout << (*it);
	}
}

std::vector<std::string>&	cleanVector(std::vector<std::string>& vect)
{
	std::vector<std::string>	res;

	for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); ++it)
	{
		const std::string&	str = (*it);
		std::string			current;

		for (size_t i = 0; i < str.length(); ++i)
		{
			char	c = str[i];
			if (isInCharset(c, "{};"))
			{
				if (!current.empty())
				{
					res.push_back(current);
					current.clear();
				}
				res.push_back(std::string(1, c));
			}
			else
				current += c;
		}

		if (!current.empty())
			res.push_back(current);
	}

	vect.swap(res);
	return vect;
}

bool	isNumber(const std::string& nbstr)
{
	for (std::string::const_iterator it = nbstr.begin(); it != nbstr.end(); ++it)
	{
		if (!isInCharset(*it, "1234567890"))
			return false;
	}
	return true;
}

std::string vecStr(const std::vector<std::string>& v)
{
	std::stringstream s;
	s << "[";
	for (size_t i = 0; i < v.size(); ++i)
	{
		if (i)
			s << ", ";
		s << v[i];
	}
	s << "]";
	return s.str();
}

std::string vecUIntStr(const std::vector<unsigned int>& v)
{
	std::stringstream s;
	s << "[";
	for (size_t i = 0; i < v.size(); ++i)
	{
		if (i)
			s << ", ";
		s << v[i];
	}
	s << "]";
	return s.str();
}

std::string mapStr(const std::map<unsigned int, std::string>& m)
{
	std::stringstream s;
	s << "{";
	for (std::map<unsigned int, std::string>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		if (it != m.begin())
			s << ", ";
		s << it->first << ": " << it->second;
	}
	s << "}";
	return s.str();
}

std::string mapVecStr(const std::map<unsigned int, std::vector<std::string> >& m)
{
	std::stringstream s;
	s << "{";
	for (std::map<unsigned int, std::vector<std::string> >::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		if (it != m.begin())
			s << ", ";
		s << it->first << ": " << vecStr(it->second);
	}
	s << "}";
	return s.str();
}

// #########################################################
};
};
