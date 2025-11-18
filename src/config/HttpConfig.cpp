#include "config/HttpConfig.hpp"

namespace Config
{
// #########################################################

void	HttpConfig::Node_::Value::deleteData(void)
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

HttpConfig::Node_::~Node_(void)
{
	Node_	*child = first_child;
	while (child)
	{
		Node_	*next = child->next_sibling;
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

HttpConfig::Node_	*HttpConfig::createStringNode_(const std::string& str)
{
	Node_	*node = new Node_(Node_::Value::TYPE_STRING);
	node->value.data = new std::string(str);
	return node;
}

HttpConfig::Node_	*HttpConfig::createUintNode_(unsigned int value)
{
	Node_	*node = new Node_(Node_::Value::TYPE_UINT);
	node->value.data = new unsigned int(value);
	return node;
}

HttpConfig::Node_	*HttpConfig::createStringVectorNode_(void)
{
	Node_	*node = new Node_(Node_::Value::TYPE_STRING_VECTOR);
	node->value.data = new std::vector<std::string>();
	return node;
}

HttpConfig::Node_	*HttpConfig::createUintVectorNode_(void)
{
	Node_	*node = new Node_(Node_::Value::TYPE_UINT_VECTOR);
	node->value.data = new std::vector<unsigned int>();
	return node;
}

HttpConfig::Node_	*HttpConfig::createMapUintStringNode_(void)
{
	Node_	*node = new Node_(Node_::Value::TYPE_MAP_UINT_STRING);
	node->value.data = new std::map<unsigned int, std::string>();
	return node;
}

HttpConfig::Node_	*HttpConfig::createMapUintStringVectorNode_(void)
{
	Node_	*node = new Node_(Node_::Value::TYPE_MAP_UINT_STRING_VECTOR);
	node->value.data = new std::map<unsigned int, std::vector<std::string> >();
	return node;
}


// #########################################################
};
