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

vecstr_t&	cleanVector(vecstr_t& vect)
{
	vecstr_t	res;

	for (vecstr_t::iterator it = vect.begin(); it != vect.end(); ++it)
	{
		const str_t&	str = (*it);
		str_t			current;

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
				res.push_back(str_t(1, c));
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

bool	isNumber(const str_t& nbstr)
{
	for (str_t::const_iterator it = nbstr.begin(); it != nbstr.end(); ++it)
	{
		if (!isInCharset(*it, "1234567890"))
			return false;
	}
	return true;
}

// #########################################################
};
};
