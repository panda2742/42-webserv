#include "HttpConfig.hpp"
#include <iostream>
#include "Node4.hpp"
#include "util.hpp"

template <typename T>
Directive<T>::Directive(T value_, cfg::Node4 *node_): value(value_), node(node_)
{
	if (!node)
		throw std::runtime_error("Parent cannot be null.");
	if (cfg::n4u::typeToEnum_(T()) != node->value.type)
		throw std::runtime_error("The type is not corresponding.");
}

template <typename T>
Directive<T>::Directive(const Directive& other): value(other.value), node(other.node) {}

template <typename T>
Directive<T>::~Directive(void) {}

template <typename T>
template <typename R>
std::vector<Directive<R> >	Directive<T>::find(const str_t& prop_name)
{
	std::vector<cfg::Node4 *>	nodes = node->access(prop_name);
	std::vector<Directive<R> >	res;

	for (std::vector<cfg::Node4 *>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (cfg::n4u::typeToEnum_(R()) != ((*it)->value.type))
			continue;
		res.push_back(Directive<R>(*(*it)->value.getAs<R>(), *it));
	}

	return res;
}

template <typename T>
template <typename R>
R	Directive<T>::get(const str_t& prop_name)
{
	std::vector<cfg::Node4 *>	nodes = node->access(prop_name);
	R							final_value;

	for (std::vector<cfg::Node4 *>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		switch ((*it)->value.type)
		{
			case cfg::Node4::TYPE_STRING:
			{
				R	v = cfg::magic_cast<R>(*(*it)->value.getAs<str_t>());
				final_value = cfg::magic_assemble<R>(final_value, v);
				break;
			}
			case cfg::Node4::TYPE_UINT:
			{
				R	v = cfg::magic_cast<R>(*(*it)->value.getAs<uint_t>());
				final_value = cfg::magic_assemble<R>(final_value, v);
				break;
			}
			case cfg::Node4::TYPE_STRING_VECTOR:
			{
				std::cout << "user requests for vec string" << std::endl;
				for (std::vector<cfg::Node4 *>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt)
				{
					std::cout << (*(*jt)).toString() << std::endl;
				}
				R	v = cfg::magic_cast<R>(*(*it)->value.getAs<vecstr_t >());
				final_value = cfg::magic_assemble<R>(final_value, v);
				break;
			}
			case cfg::Node4::TYPE_UINT_VECTOR:
			{
				R	v = cfg::magic_cast<R>(*(*it)->value.getAs<vecuint_t >());
				final_value = cfg::magic_assemble<R>(final_value, v);
				break;
			}
			case cfg::Node4::TYPE_MAP_UINT_STRING:
			{
				R	v = cfg::magic_cast<R>(*(*it)->value.getAs<mapstr_t >());
				final_value = cfg::magic_assemble<R>(final_value, v);
				break;
			}
			case cfg::Node4::TYPE_MAP_UINT_STRING_VECTOR:
			{
				R	v = cfg::magic_cast<R>(*(*it)->value.getAs<mapvec_t>());
				final_value = cfg::magic_assemble<R>(final_value, v);
				break;
			}
		}
	}

	return final_value;
}

template <typename T>
std::ostream&	operator<<(std::ostream &os, const Directive<T>& directive)
{
	os << "Directive { " << cfg::util::represent(directive.value) << " }";
	return os;
}

namespace cfg
{
// #########################################################

template <typename T>
HttpConfig::GetData<T>::GetData(const str_t& prop_name_, const Node4 *parent_)
	: parent(parent_), prop_name(prop_name_) {}

template <typename T>HttpConfig::GetData<T>::GetData(const GetData& other)
	: parent(other.parent),
	  prop_name(other.prop_name),
	  local_res(other.local_res) {}

template <typename T>HttpConfig::GetData<T>::~GetData(void) {}

template <typename T>
std::vector<Directive<T> >	HttpConfig::get_(GetData<T>& ddata)
{
	std::vector<GetData<T> >	results;

	Node4	*c = ddata.parent->first_child;
	while (c)
	{
		if (c->name == ddata.prop_name)
		{
			GetData<T>	directive_content(ddata);
			++directive_content.depth;
			directive_content.parent = c;
			directive_content.local_res.push_back(Directive<T>(*c->value.getAs<T>(), c));
			results.push_back(directive_content);
		}
		c = c->next_sibling;
	}

	if (!results.size())
	{
		c = ddata.parent->parent->first_child;
		while (c->prev_sibling)
			c = c->prev_sibling;
		while (c)
		{
			if (c->name == ddata.prop_name)
			{
				GetData<T>	directive_content(ddata);
				++directive_content.depth;
				directive_content.parent = c;
				directive_content.local_res.push_back(Directive<T>(*c->value.getAs<T>(), c));
				results.push_back(directive_content);
			}
			c = c->next_sibling;
		}
	}

	for (typename std::vector<GetData<T> >::const_iterator it = results.begin(); it != results.end(); ++it)
	{
		ddata.local_res.insert(ddata.local_res.end(), (*it).local_res.begin(), (*it).local_res.end());
	}
	return ddata.local_res;
}

template <typename T>
std::vector<Directive<T> >	HttpConfig::get(const str_t& prop_name, const Node4 *parent)
{
	if (!parent)
		parent = root_;

	GetData<T>	ddata(prop_name, parent, 0);
	std::vector<Directive<T> >	directives = get_(ddata);
	return directives;
}

template <typename T, typename P>
std::vector<Directive<T> >	HttpConfig::get(const str_t& prop_name, const Directive<P>& directive)
{
	return get(prop_name, directive.node);
}

// #########################################################
};
