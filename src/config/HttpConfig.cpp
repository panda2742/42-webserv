#include "config/HttpConfig.hpp"
#include <iostream>
#include "config/ConfigLogger.hpp"
#include "config/Utils.hpp"

namespace Config
{
// #########################################################

void	HttpConfig::Node_::Value::deleteData(void)
{
	switch (type)
	{
		case TYPE_NULL:
			break;
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

HttpConfig::Node_	*HttpConfig::createNullNode_(void)
{
	Node_	*node = new Node_(Node_::Value::TYPE_NULL);
	node->value.data = NULL;
	return node;
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

HttpConfig::Node_::Value::DataType	HttpConfig::dataType_(std::vector<Lexer::TokenNode>::const_iterator node, std::vector<Lexer::TokenNode>::const_iterator end)
{
	if (node == end || (node->type != Lexer::TokenParent && node->type != Lexer::TokenDirective))
	{
		log.error("Trying to access the type of a non-directive attribute '" + node->value + "'.");
		return Node_::Value::TYPE_NULL;
	}
	log.debug("Directive key name: " + node->value);
	bool					nb_starts = false;
	bool					only_nb = true;
	unsigned int			nb_arguments = 0;
	++node;
	const std::vector<Lexer::TokenNode>::const_iterator	start = node;
	while (node != end)
	{
		if (node->type != Lexer::TokenArgument)
			break;
		++nb_arguments;
		if (Utils::isNumber(node->value) && start == node)
			nb_starts = true;
		else
		{
			if (start != node)
				only_nb = false;
		}
		log.info("Argument: " + node->value);
		++node;
	}
	if (nb_arguments == 0)
		return Node_::Value::TYPE_NULL;
	if (nb_starts)
	{
		if (nb_arguments == 1)
			return Node_::Value::TYPE_UINT;
		else if (only_nb)
			return Node_::Value::TYPE_UINT_VECTOR;
		else if (nb_arguments == 2)
			return Node_::Value::TYPE_MAP_UINT_STRING;
		else
			return Node_::Value::TYPE_MAP_UINT_STRING_VECTOR;
	}
	else
	{
		if (nb_arguments == 1)
			return Node_::Value::TYPE_STRING;
		else
			return Node_::Value::TYPE_STRING_VECTOR;
	}
	log.warn("Could not identify type for directive '" + node->value + "'.");
	return Node_::Value::TYPE_NULL;
}

void	HttpConfig::generate(const std::vector<Lexer::TokenNode>& nodes) throw(ParsingException)
{
	for (std::vector<Lexer::TokenNode>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (it->type == Lexer::TokenParent || it->type == Lexer::TokenDirective)
		{
			HttpConfig::Node_::Value::DataType	type = dataType_(it, nodes.end());
			std::cout << "Identified type is " RED << type << RESET << std::endl << std::endl;
		}
	}
}

// #########################################################
};
