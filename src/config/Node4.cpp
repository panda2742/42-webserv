#include "config/Node4.hpp"

#include <sstream>
#include <map>
#include "config/types.hpp"
#include "config/Node4.hpp"
#include "config/ContainerImproved.hpp"

namespace Config
{
// #########################################################

Node4::Value::Value(void) {}

Node4::Value::~Value(void) {}

Node4::Node4(ValueType type_id)
{
	value.type = type_id;
	parent = NULL;
	first_child = NULL;
	next_sibling = NULL;
	prev_sibling = NULL;
}

Node4::~Node4(void)
{
	Node4	*child = first_child;
	while (child)
	{
		Node4	*next = child->next_sibling;
		child->parent = NULL;
		child->prev_sibling = NULL;
		child->next_sibling = NULL;
		delete child;
		child = next;
	}
	first_child = NULL;

	if (prev_sibling != NULL)
		prev_sibling->next_sibling = next_sibling;
	if (next_sibling != NULL)
		next_sibling->prev_sibling = prev_sibling;

	if (parent != NULL && parent->first_child == this)
		parent->first_child = next_sibling;

	value.deleteData();
}

void	Node4::Value::deleteData(void)
{
	switch (type)
	{
		case TYPE_STRING:
			delete getAs<std::string>();
			break;
		case TYPE_UINT:
			delete getAs<unsigned int>();
			break;
		case TYPE_STRING_VECTOR:
			delete getAs<std::vector<std::string> >();
			break;
		case TYPE_UINT_VECTOR:
			delete getAs<std::vector<unsigned int> >();
			break;
		case TYPE_MAP_UINT_STRING:
			delete getAs<std::map<unsigned int, std::string> >();
			break;
		case TYPE_MAP_UINT_STRING_VECTOR:
			delete getAs<std::map<unsigned int, std::vector<std::string> > >();
			break;
		default:
			break;
	}
	data = NULL;
}

std::string	Node4::fastStr(void)
{
	std::stringstream	ss;

	if (parent)
		ss << BLURPLE "" << parent->name << RESET " |";
	ss << " " RED << name << " " RESET;

	switch (value.type)
	{
		case TYPE_STRING:
			ss << ORANGE << "STRING" << RESET << "=" << ORANGE << *value.getAs<std::string>() << RESET;
			break;
		case TYPE_UINT:
			ss << LIGHT_GREEN << "UINT" << RESET << "=" << LIGHT_GREEN << *value.getAs<unsigned int>() << RESET;
			break;
		case TYPE_STRING_VECTOR:
			ss << GREEN << "STRINGVEC" << RESET << "=" << GREEN << ContainerImproved::vecStr(*value.getAs<std::vector<std::string> >()) << RESET;
			break;
		case TYPE_UINT_VECTOR:
			ss << CYAN << "UINTVEC" << RESET << "=" << CYAN << ContainerImproved::vecUIntStr(*value.getAs<std::vector<unsigned int> >()) << RESET;
			break;
		case TYPE_MAP_UINT_STRING:
			ss << BLURPLE << "UINT_STRING" << RESET << "=" << BLURPLE << ContainerImproved::mapStr(*value.getAs<std::map<unsigned int, std::string> >()) << RESET;
			break;
		case TYPE_MAP_UINT_STRING_VECTOR:
			ss << PINK << "UINT_STRINGVEC" << RESET << "=" << PINK << ContainerImproved::mapVecStr(*value.getAs<std::map<unsigned int, std::vector<std::string> > >()) << RESET;
			break;
		default:
			ss << GREY << " EMPTY" << RESET;
			break;
	}
	if (next_sibling)
		ss << GREY "\t\t-> " << next_sibling->name << " ";
	ss << RESET;
	ss << std::endl;

	return ss.str();
}

std::string Node4::toString(void)
{
	unsigned int	tabs = 0;
	Node4	*p = parent;
	while (p)
	{
		p = p->parent;
		++tabs;
	}
    std::stringstream	ss;
    ss << std::string(tabs, '\t') << fastStr();

	if (first_child)
		ss << first_child->toString();

    if (next_sibling)
        ss << next_sibling->toString();
	else
		ss << std::endl;

    return ss.str();
}

std::vector<Node4 *>	Node4::access(const std::string& child_name)
{
	std::vector<Node4 *>	res;

	Node4	*c = this->first_child;
	while (c)
	{
		if (c->name == child_name)
			res.push_back(c);
		c = c->next_sibling;
	}

	return res;
}

std::vector<Node4 *>	Node4::access(const std::string& child_name) const
{
	std::vector<Node4 *>	res;

	Node4	*c = this->first_child;
	while (c)
	{
		if (c->name == child_name)
			res.push_back(c);
		c = c->next_sibling;
	}

	return res;
}

// #########################################################
};


