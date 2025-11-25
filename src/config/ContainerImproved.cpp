#include "config/Node4.hpp"
#include "config/Lexer.hpp"
#include <sstream>

namespace Config
{
namespace ContainerImproved
{
// #########################################################

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
