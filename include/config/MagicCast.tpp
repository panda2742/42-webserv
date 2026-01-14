#include "MagicCast.hpp"
#include "Node4.hpp"
#include <typeinfo>
#include <iostream>
#include <sstream>
#include "util.hpp"

namespace cfg
{
// #########################################################

/*
====== STRING ===========================
r_type: 0
r_type typeid: NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE

====== uint_t =====================
r_type: 1
r_type typeid: j

====== VECTOR STRING ====================
r_type: 2
r_type typeid: St6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS5_EE

====== VECTOR uint_t ==============
r_type: 3
r_type typeid: St6vectorIjSaIjEE

====== MAP uint_t STRING ==========
r_type: 4
r_type typeid: St3mapIjNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt4lessIjESaISt4pairIKjS5_EEE

====== MAP uint_t VECTOR STRING ===
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
	Node4::ValueType	r_type = n4u::typeToEnum_(R()),
						t_type = n4u::typeToEnum_(T());

	if (r_type == t_type)
		return as<R>(value);

	std::stringstream			string;
	uint_t						uint;
	vecstr_t					vector_string;
	vecuint_t					vector_uint;
	mapstr_t					map1;
	mapvec_t					map2;

	switch (t_type)
	{
		case Node4::TYPE_STRING:
			if (r_type != Node4::TYPE_STRING_VECTOR)
				throw InvalidStringException();
			vector_string.push_back(as<str_t>(value));
			return as<R>(vector_string);
			break;

		case Node4::TYPE_UINT:
		{
			if (r_type == Node4::TYPE_STRING)
			{
				string << as<uint_t>(value);
				return as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				string << as<uint_t>(value);
				vector_string.push_back(string.str());
				return as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_UINT_VECTOR)
			{
				vector_uint.push_back(as<uint_t>(value));
				return as<R>(vector_uint);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING)
			{
				map1.insert(std::make_pair(as<uint_t>(value), ""));
				return as<R>(map1);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				map2.insert(std::make_pair(as<uint_t>(value), vecstr_t()));
				return as<R>(map2);
			}
			break;
		}

		case Node4::TYPE_STRING_VECTOR:
		{
			if (r_type != Node4::TYPE_STRING)
				throw InvalidVectorStringException();
			vecstr_t	value_vec = as<vecstr_t >(value);
			for (vecstr_t::const_iterator it = value_vec.begin(); it != value_vec.end(); ++it)
				string << (*it) + " ";
			return as<R>(string.str());
			break;
		}

		case Node4::TYPE_UINT_VECTOR:
		{
			if (r_type == Node4::TYPE_UINT)
				throw InvalidVectorUintException();
			vecuint_t	value_vec = as<vecuint_t >(value);
			if (r_type == Node4::TYPE_STRING)
			{
				for (vecuint_t::const_iterator it = value_vec.begin(); it != value_vec.end(); ++it)
					string << (*it) << " ";
				return	as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				for (vecuint_t::const_iterator it = value_vec.begin(); it != value_vec.end(); ++it)
				{
					string.clear();
					string.str("");
					string << (*it);
					vector_string.push_back(string.str());
				}
				return	as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING)
			{
				vecuint_t::const_iterator	it = value_vec.begin();
				uint = (*it);
				++it;
				for (; it != value_vec.end(); ++it)
					string << (*it) << " ";
				map1.insert(std::make_pair(uint, string.str()));
				return	as<R>(map1);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				vecuint_t::const_iterator	it = value_vec.begin();
				uint = (*it);
				++it;
				for (; it != value_vec.end(); ++it)
				{
					string.clear();
					string.str("");
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
			mapstr_t	value_map = as<mapstr_t >(value);
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
			mapvec_t	value_map = as<mapvec_t>(value);
			if (r_type == Node4::TYPE_STRING)
			{
				if (value_map.size() == 0)
					return as<R>(string.str());
				string << value_map.begin()->first << " ";
				for (vecstr_t::const_iterator it = value_map.begin()->second.begin(); it != value_map.begin()->second.end(); ++it)
					string << (*it) << " ";
				return as<R>(string.str());
			}
			else if (r_type == Node4::TYPE_STRING_VECTOR)
			{
				if (value_map.size() == 0)
					return as<R>(vector_string);
				string << value_map.begin()->first;
				vector_string.push_back(string.str());
				for (vecstr_t::const_iterator it = value_map.begin()->second.begin(); it != value_map.begin()->second.end(); ++it)
					vector_string.push_back(*it);
				return as<R>(vector_string);
			}
			else if (r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR)
			{
				if (value_map.size() == 0)
					return as<R>(map1);
				for (vecstr_t::const_iterator it = value_map.begin()->second.begin(); it != value_map.begin()->second.end(); ++it)
					string << (*it) << " ";
				map1.insert(std::make_pair(value_map.begin()->first, string.str()));
				return as<R>(map1);
			}
			break;
		}
	}
	return R();
}

template <typename R, typename A, typename B>
R	magic_assemble(A& a, B& b) throw(MagicCastException)
{
	Node4::ValueType	a_type = n4u::typeToEnum_(A());

	if (a_type == Node4::TYPE_STRING || a_type == Node4::TYPE_UINT)
		throw AssembleTooPrimitive();

	R	converted_a = magic_cast<R>(a),
		converted_b = magic_cast<R>(b);

	converted_a = magic_assemble_sub_(converted_a, converted_b);

	return converted_a;
}

// #########################################################
};
