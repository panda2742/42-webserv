#include "Location.hpp"

template <typename T>
bool	Location::vecCmp_(std::vector<T>& vec1, std::vector<T>& vec2)
{
	typename std::vector<T>::const_iterator	it = vec1.begin(), jt = vec2.begin();

	while (it != vec1.end() && jt != vec2.end() && (*it) == (*jt))
	{
		++it;
		++jt;
	}

	return it == vec1.end() && jt == vec2.end();
}

template <typename T>
std::vector<T> Location::vecConsume_(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
	std::vector<T> result = vec2;
	size_t i = 0;

	while (i < vec1.size() && i < result.size() && vec1[i] == result[i])
		++i;

	result.erase(result.begin(), result.begin() + i);
	return result;
}
