#include "HttpConfig.hpp"
#include <iostream>
#include "config/Node4.hpp"

template <typename T>
Directive<T>::Directive(T value_, Config::Node4 *node_): value(value_), node(node_)
{
	if (node || Config::Node4Utils::typeToEnum_(T()) != node->value.type)
		throw std::runtime_error("The type is not corresponding.");
}

template <typename T>
Directive<T>::Directive(const Directive& other): value(other.value), node(other.node) {}

template <typename T>
Directive<T>::~Directive(void) {}

template <typename T>
template <typename R>
std::vector<Directive<R> >	Directive<T>::find(const std::string& prop_name)
{
	std::vector<Config::Node4 *>	nodes = node->access(prop_name);
	std::vector<Directive<R> >		res;

	for (std::vector<Config::Node4 *>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (Config::Node4Utils::typeToEnum_(R()) != ((*it)->value.type))
			throw std::runtime_error("The type is not corresponding.");
		res.push_back(Directive<R>(*(*it)->value.getAs<R>(), *it));
	}

	return res;
}

namespace Config
{
// #########################################################

template <typename T>
HttpConfig::GetData<T>::GetData(const std::string& prop_name_, const Node4 *parent_, unsigned int depth_)
	: depth(depth_), parent(parent_), prop_name(prop_name_)
{
	type = Node4Utils::typeToEnum_(T());
}

template <typename T>HttpConfig::GetData<T>::GetData(const GetData& other)
	: depth(other.depth),
	  parent(other.parent),
	  type(other.type),
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
std::vector<Directive<T> >	HttpConfig::get(const std::string& prop_name, const Node4 *parent)
{
	if (!parent)
		parent = root_;

	GetData<T>	ddata(prop_name, parent, 0);
	return get_(ddata);
}

template <typename T, typename P>
std::vector<Directive<T> >	HttpConfig::get(const std::string& prop_name, const Directive<P>& directive)
{
	return get(prop_name, directive.node);
}

// #########################################################
};
