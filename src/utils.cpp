#include "utils.hpp"
#include <string>
#include <vector>

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

std::string trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t\r\n");
	if (start == std::string::npos) return "";
	size_t end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char sep)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t i;

	for (i = 0; i < str.size(); ++i)
	{
		if (str[i] == sep)
		{
			if (i - start > 0) result.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}

	result.push_back(str.substr(start));
	return result;
}

