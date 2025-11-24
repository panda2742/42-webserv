#include "HttpConfig.hpp"
#include <iostream>

namespace Config
{
// #########################################################

template <typename T>
std::vector<HttpConfig::Node_t<T> >	HttpConfig::get_(DepthData_<T>& ddata)
{
	std::vector<DepthData_<T> >	results;

	Node_	*c = ddata.parent->first_child;
	while (c)
	{
		if (c->name == ddata.prop_name)
		{
			DepthData_<T>	directive_content(ddata);
			++directive_content.depth;
			directive_content.parent = c;
			directive_content.local_res.push_back(Node_t<T>(*c->value.getAs<T>(), c));
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
				DepthData_<T>	directive_content(ddata);
				++directive_content.depth;
				directive_content.parent = c;
				directive_content.local_res.push_back(Node_t<T>(*c->value.getAs<T>(), c));
				results.push_back(directive_content);
			}
			c = c->next_sibling;
		}
	}

	for (typename std::vector<DepthData_<T> >::const_iterator it = results.begin(); it != results.end(); ++it)
	{
		ddata.local_res.insert(ddata.local_res.end(), (*it).local_res.begin(), (*it).local_res.end());
	}
	return ddata.local_res;
}

template <typename T>
std::vector<HttpConfig::Node_t<T> >	HttpConfig::get(const std::string& prop_name, const Node_ *parent)
{
	if (!parent)
		parent = root_;

	DepthData_<T>	ddata(prop_name, parent, 0);
	return get_(ddata);
}

// #########################################################
};
