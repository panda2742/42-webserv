#ifndef CONTAINER_IMPROVED_HPP
#define CONTAINER_IMPROVED_HPP

#include <string>

namespace Config
{
namespace ContainerImproved
{
// #########################################################

std::string	vecStr(const std::vector<std::string>& v);
std::string	vecUIntStr(const std::vector<unsigned int>& v);
std::string	mapStr(const std::map<unsigned int, std::string>& m);
std::string	mapVecStr(const std::map<unsigned int, std::vector<std::string> >& m);

// #########################################################
};
};

#endif