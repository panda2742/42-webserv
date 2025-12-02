#include "MagicCast.hpp"
#include "Node4.hpp"
#include <typeinfo>
#include <iostream>

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

template <typename T>
static T	magic_transform_(const void *v_ptr)
{
	return T(*const_cast<T *>(reinterpret_cast<const T *>(v_ptr)));
}

template <typename R, typename T>
R	magic_cast(T value) throw(MagicCastException)
{
	const std::string	r_id = typeid(R).name(),
						t_id = typeid(T).name();

	Node4::ValueType	r_type = Node4Utils::typeToEnum_(R()),
						t_type = Node4Utils::typeToEnum_(T());

	if (r_type == t_type)
		return magic_transform_<R>(reinterpret_cast<void *>(&value));

	bool	compatibility_check = r_type == Node4::TYPE_UINT || r_type == Node4::TYPE_UINT_VECTOR || r_type == Node4::TYPE_MAP_UINT_STRING || r_type == Node4::TYPE_MAP_UINT_STRING_VECTOR;
	switch (t_type)
	{
		case Node4::TYPE_STRING:
			if (compatibility_check)
				throw InvalidCastException();
			break;

		case Node4::TYPE_UINT:
			break;

		case Node4::TYPE_STRING_VECTOR:
			if (compatibility_check)
				throw InvalidCastException();
			break;

		case Node4::TYPE_UINT_VECTOR:
			break;

		case Node4::TYPE_MAP_UINT_STRING:
			break;

		case Node4::TYPE_MAP_UINT_STRING_VECTOR:
			break;
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
