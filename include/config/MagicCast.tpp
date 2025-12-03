#include "MagicCast.hpp"
#include "Node4.hpp"
#include <typeinfo>
#include <iostream>
#include <sstream>

namespace Config
{
// #########################################################

/*
====== STRING ===========================
r_type: 0
r_type typeid: NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE

====== UNSIGNED INT =====================
r_type: 1
r_type typeid: j

====== VECTOR STRING ====================
r_type: 2
r_type typeid: St6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS5_EE

====== VECTOR UNSIGNED INT ==============
r_type: 3
r_type typeid: St6vectorIjSaIjEE

====== MAP UNSIGNED INT STRING ==========
r_type: 4
r_type typeid: St3mapIjNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt4lessIjESaISt4pairIKjS5_EEE

====== MAP UNSIGNED INT VECTOR STRING ===
r_type: 5
r_type typeid: St3mapIjSt6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS6_EESt4lessIjESaISt4pairIKjS8_EEE
 */

template <typename A, typename B>
static A	as(B value)
{
	return A(*const_cast<A *>(reinterpret_cast<const A *>(reinterpret_cast<const void *>(&value))));
}

template <typename R, typename T>
R	magic_cast(T value) throw(MagicCastException)
{
	Node4::ValueType	r_type = Node4Utils::typeToEnum_(R()),
						t_type = Node4Utils::typeToEnum_(T());

	if (r_type == t_type)
		return as<R>(value);

	const std::string	r_id = typeid(R).name(),
						t_id = typeid(T).name();

	std::stringstream									string;
	unsigned int										uint;
	std::vector<std::string>							vector_string;
	std::vector<unsigned int>							vector_uint;
	std::map<unsigned int, std::string>					map1;
	std::map<unsigned int, std::vector<std::string> >	map2;

	(void)uint;
	(void)vector_string;
	(void)vector_uint;
	(void)map1;
	(void)map2;

	switch (t_type)
	{
		case Node4::TYPE_STRING:
			if (r_type != Node4::TYPE_STRING_VECTOR)
				throw InvalidStringException();
			vector_string.push_back(as<std::string>(value));
			return as<R>(vector_string);
			break;

		case Node4::TYPE_UINT:
		{
			if (r_type == Node4::TYPE_STRING)
			{
				string << as<unsigned int>(value);
				return as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				string << as<unsigned int>(value);
				vector_string.push_back(string.str());
				return as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_UINT_VECTOR)
			{
				vector_uint.push_back(as<unsigned int>(value));
				return as<R>(vector_uint);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING)
			{
				map1.insert(std::make_pair(as<unsigned int>(value), ""));
				return as<R>(map1);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				map2.insert(std::make_pair(as<unsigned int>(value), std::vector<std::string>()));
				return as<R>(map2);
			}
			break;
		}

		case Node4::TYPE_STRING_VECTOR:
		{
			if (r_type != Node4::TYPE_STRING)
				throw InvalidVectorStringException();
			std::vector<std::string>	value_vec = as<std::vector<std::string> >(value);
			for (std::vector<std::string>::const_iterator it = value_vec.begin(); it != value_vec.end(); ++it)
				string << (*it) + " ";
			return as<R>(string.str());
			break;
		}

		case Node4::TYPE_UINT_VECTOR:
		{
			if (r_type == Node4::TYPE_UINT)
				throw InvalidVectorUintException();
			std::vector<unsigned int>	value_vec = as<std::vector<unsigned int> >(value);
			if (r_type == Node4::TYPE_STRING)
			{
				for (std::vector<unsigned int>::const_iterator it = value_vec.begin(); it != value_vec.end(); ++it)
					string << (*it) << " ";
				return	as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				for (std::vector<unsigned int>::const_iterator it = value_vec.begin(); it != value_vec.end(); ++it)
				{
					string.clear();
					string << (*it);
					vector_string.push_back(string.str());
				}
				return	as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING)
			{
				std::vector<unsigned int>::const_iterator	it = value_vec.begin();
				uint = (*it);
				++it;
				for (; it != value_vec.end(); ++it)
					string << (*it) << " ";
				map1.insert(std::make_pair(uint, string.str()));
				return	as<R>(map1);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				std::vector<unsigned int>::const_iterator	it = value_vec.begin();
				uint = (*it);
				++it;
				for (; it != value_vec.end(); ++it)
				{
					string.clear();
					string << (*it);
					vector_string.push_back(string.str());
				}
				map2.insert(std::make_pair(uint, vector_string));
				return	as<R>(map2);
			}
			break;
		}

		case Node4::TYPE_MAP_UINT_STRING:
		{
			if (r_type != Node4::TYPE_STRING && r_type != Node4::TYPE_STRING_VECTOR && r_type != Node4::TYPE_MAP_UINT_STRING_VECTOR)
				throw InvalidMapStringException();
			std::map<unsigned int, std::string>	value_map = as<std::map<unsigned int, std::string> >(value);
			if (r_type == Node4::TYPE_STRING)
			{
				if (value_map.size() == 0)
					return as<R>(string.str());
				string << value_map.begin()->first << " " << value_map.begin()->second;
				return as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				if (value_map.size() == 0)
					return as<R>(vector_string);
				string << value_map.begin()->first;
				vector_string.push_back(string.str());
				vector_string.push_back(value_map.begin()->second);
				return as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				if (value_map.size() == 0)
					return as<R>(map2);
				vector_string.push_back(value_map.begin()->second);
				map2.insert(std::make_pair(value_map.begin()->first, vector_string));
				return as<R>(map2);
			}
			break;
		}

		case Node4::TYPE_MAP_UINT_STRING_VECTOR:
		{
			if (r_type != Node4::TYPE_STRING && r_type != Node4::TYPE_STRING_VECTOR && r_type != Node4::TYPE_MAP_UINT_STRING)
				throw InvalidMapVectorStringException();
			std::map<unsigned int, std::vector<std::string> >	value_map = as<std::map<unsigned int, std::vector<std::string> > >(value);
			if (r_type == Node4::TYPE_STRING)
			{
				if (value_map.size() == 0)
					return as<R>(string.str());
				string << value_map.begin()->first << " ";
				for (std::vector<std::string>::const_iterator it = value_map.begin()->second.begin(); it != value_map.begin()->second.end(); ++it)
					string << (*it) << " ";
				return as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				if (value_map.size() == 0)
					return as<R>(vector_string);
				string << value_map.begin()->first;
				vector_string.push_back(string.str());
				for (std::vector<std::string>::const_iterator it = value_map.begin()->second.begin(); it != value_map.begin()->second.end(); ++it)
					vector_string.push_back(*it);
				return as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				if (value_map.size() == 0)
					return as<R>(map1);
				for (std::vector<std::string>::const_iterator it = value_map.begin()->second.begin(); it != value_map.begin()->second.end(); ++it)
					string << (*it) << " ";
				map1.insert(std::make_pair(value_map.begin()->first, string.str()));
				return as<R>(map1);
			}
			break;
			break;
		}
	}
	return R();
}

template <typename A, typename B>
A&	magic_refcpy(A& a, B& b) throw(MagicCastException)
{
	(void)a;
	(void)b;
}

template <typename A, typename B>
A	magic_cpy(A& a, B& b) throw(MagicCastException)
{
	(void)a;
	(void)b;
}

// #########################################################
};
