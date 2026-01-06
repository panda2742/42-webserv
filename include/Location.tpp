#include "Location.hpp"

template <typename T>
bool	Location::vecCmp_(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
	typename std::vector<T>::const_iterator	it = vec1.begin(), jt = vec2.begin();

	while (it != vec1.end() && jt != vec2.end() && (*it) == (*jt))
	{
		++it;
		++jt;
	}

	return it == vec1.end() && jt == vec2.end();
}