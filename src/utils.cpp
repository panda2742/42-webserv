#include "utils.hpp"

std::string getNextPart(std::string& input, const std::string& sep)
{
	size_t next_space = input.find(sep);
	std::string res;

	if (next_space == std::string::npos)
	{
		res = input.substr(0, input.size());
		input.erase(0, input.size());
	}
	else
	{
		res = input.substr(0, next_space);
		input.erase(0, next_space + 1);
	}
	return res;
}

std::string toUpper(const std::string& src)
{
	std::string str = src;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z') str[i] -= 32;
		else if (str[i] == '-') str[i] = '_';
	}
	return str;
}
