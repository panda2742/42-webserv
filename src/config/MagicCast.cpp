#include "config/MagicCast.hpp"

namespace cfg
{
// #########################################################

MagicCastException::MagicCastException(const std::string& err_msg): std::runtime_error(err_msg) {}

InvalidStringException::InvalidStringException(void): MagicCastException(INVALID_STRING_EXCEPTION) {}

InvalidVectorStringException::InvalidVectorStringException(void): MagicCastException(INVALID_VECTOR_STRING_EXCEPTION) {}

InvalidVectorUintException::InvalidVectorUintException(void): MagicCastException(INVALID_VECTOR_UINT_EXCEPTION) {}

InvalidMapStringException::InvalidMapStringException(void): MagicCastException(INVALID_MAP_STRING_EXCEPTION) {}

InvalidMapVectorStringException::InvalidMapVectorStringException(void): MagicCastException(INVALID_MAP_VECTOR_STRING_EXCEPTION) {}

AssembleTooPrimitive::AssembleTooPrimitive(void): MagicCastException(ASSEMBLE_TOO_PRIMITIVE) {}

std::vector<std::string>							magic_assemble_sub_(std::vector<std::string>& a, std::vector<std::string>& b) throw (MagicCastException)
{
	std::vector<std::string>::const_iterator	it = b.begin();
	for (; it != b.end(); ++it)
		a.push_back(*it);
	return a;
}

std::vector<unsigned int>							magic_assemble_sub_(std::vector<unsigned int>& a, std::vector<unsigned int>& b) throw (MagicCastException)
{
	std::vector<unsigned int>::const_iterator	it = b.begin();
	for (; it != b.end(); ++it)
		a.push_back(*it);
	return a;
}

std::map<unsigned int, std::string>					magic_assemble_sub_(std::map<unsigned int, std::string>& a, std::map<unsigned int, std::string>& b) throw (MagicCastException)
{
	std::map<unsigned int, std::string>::const_iterator	it = b.begin();
	for (; it != b.end(); ++it)
		a.insert(*it);
	return a;
}

std::map<unsigned int, std::vector<std::string> >	magic_assemble_sub_(std::map<unsigned int, std::vector<std::string> >& a, std::map<unsigned int, std::vector<std::string> >& b) throw (MagicCastException)
{
	std::map<unsigned int, std::vector<std::string> >::const_iterator	it = b.begin();
	for (; it != b.end(); ++it)
		a.insert(*it);
	return a;
}


// #########################################################
};
