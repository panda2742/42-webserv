#include "config/Node4.hpp"

#include <sstream>
#include <map>
#include "config/types.hpp"
#include "config/Node4.hpp"
#include "config/util.hpp"

namespace cfg
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
			delete getAs<str_t>();
			break;
		case TYPE_UINT:
			delete getAs<uint_t>();
			break;
		case TYPE_STRING_VECTOR:
			delete getAs<vecstr_t >();
			break;
		case TYPE_UINT_VECTOR:
			delete getAs<vecuint_t >();
			break;
		case TYPE_MAP_UINT_STRING:
			delete getAs<mapstr_t >();
			break;
		case TYPE_MAP_UINT_STRING_VECTOR:
			delete getAs<mapvec_t>();
			break;
		default:
			break;
	}
	data = NULL;
}

str_t	Node4::fastStr_(void)
{
	std::stringstream	ss;

	if (parent)
		ss << BLURPLE "" << parent->name << RESET " |";
	ss << " " RED << name << " " RESET;

	switch (value.type)
	{
		case TYPE_STRING:
			ss << "STRING =" << util::represent(*value.getAs<str_t>());
			break;
		case TYPE_UINT:
			ss << "UINT =" << util::represent(*value.getAs<uint_t>());
			break;
		case TYPE_STRING_VECTOR:
			ss << "STRINGVEC =" << util::represent(*value.getAs<vecstr_t >());
			break;
		case TYPE_UINT_VECTOR:
			ss << "UINTVEC =" << util::represent(*value.getAs<vecuint_t >());
			break;
		case TYPE_MAP_UINT_STRING:
			ss << "UINT_STRING =" << util::represent(*value.getAs<mapstr_t >());
			break;
		case TYPE_MAP_UINT_STRING_VECTOR:
			ss << "UINT_STRINGVEC =" << util::represent(*value.getAs<mapvec_t>());
			break;
		default:
			ss << " EMPTY";
			break;
	}
	if (next_sibling)
		ss << GREY "\t\t-> " << next_sibling->name << " ";
	ss << RESET;
	ss << std::endl;

	return ss.str();
}

str_t Node4::toString(void)
{
	uint_t	tabs = 0;
	Node4	*p = parent;
	while (p)
	{
		p = p->parent;
		++tabs;
	}
    std::stringstream	ss;
    ss << str_t(tabs, '\t') << fastStr_();

	if (first_child)
		ss << first_child->toString();

    if (next_sibling)
        ss << next_sibling->toString();
	else
		ss << std::endl;

    return ss.str();
}

std::vector<Node4 *>	Node4::access(const str_t& child_name)
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


