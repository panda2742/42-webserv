#include "config/Node4.hpp"
#include "config/ConfigLogger.hpp"
#include "config/Utils.hpp"

namespace Config
{
namespace Node4Utils
{
// #########################################################

Node4	*createNode4(Node4::ValueType type)
{
	try
	{
		switch (type)
		{
			case Node4::TYPE_STRING:
				return createStringNode4();
				break;
			case Node4::TYPE_UINT:
				return createUintNode4();
				break;
			case Node4::TYPE_STRING_VECTOR:
				return createStringVectorNode4();
				break;
			case Node4::TYPE_UINT_VECTOR:
				return createUintVectorNode4();
				break;
			case Node4::TYPE_MAP_UINT_STRING:
				return createMapUintStringNode4();
				break;
			case Node4::TYPE_MAP_UINT_STRING_VECTOR:
				return createMapUintStringVectorNode4();
				break;
			default:
				return NULL;
				break;
		}
	}
	catch (const std::bad_alloc& e)
	{
		log.error("Memory allocation failed in createNode4: " + std::string(e.what()));
		return NULL;
	}
	return NULL;
}

Node4	*createStringNode4()
{
	Node4	*node = new Node4(Node4::TYPE_STRING);
	node->value.data = new std::string();
	return node;
}

Node4	*createUintNode4()
{
	Node4	*node = new Node4(Node4::TYPE_UINT);
	node->value.data = new unsigned int();
	return node;
}

Node4	*createStringVectorNode4(void)
{
	Node4	*node = new Node4(Node4::TYPE_STRING_VECTOR);
	node->value.data = new std::vector<std::string>();
	return node;
}

Node4	*createUintVectorNode4(void)
{
	Node4	*node = new Node4(Node4::TYPE_UINT_VECTOR);
	node->value.data = new std::vector<unsigned int>();
	return node;
}

Node4	*createMapUintStringNode4(void)
{
	Node4	*node = new Node4(Node4::TYPE_MAP_UINT_STRING);
	node->value.data = new std::map<unsigned int, std::string>();
	return node;
}

Node4	*createMapUintStringVectorNode4(void)
{
	Node4	*node = new Node4(Node4::TYPE_MAP_UINT_STRING_VECTOR);
	node->value.data = new std::map<unsigned int, std::vector<std::string> >();
	return node;
}

Node4::ValueType	dataType_(std::vector<Lexer::TokenNode>::const_iterator node, std::vector<Lexer::TokenNode>::const_iterator end)
{
	if (node == end || (node->type != Lexer::TokenParent && node->type != Lexer::TokenDirective))
	{
		log.error("Trying to access the type of a non-directive attribute '" + node->value + "'.");
		return Node4::TYPE_STRING;
	}
	bool					nb_starts = false;
	bool					only_nb = true;
	unsigned int			nb_arguments = 0;
	const std::vector<Lexer::TokenNode>::const_iterator	start = ++node;
	std::vector<Lexer::TokenNode>::const_iterator		current = start;
	while (current != end && current->type == Lexer::TokenArgument)
	{
		++nb_arguments;
		if (Utils::isNumber(current->value) && start == current)
			nb_starts = true;
		else if (start != current)
			only_nb = false;

		++current;
	}
	if (nb_arguments == 0)
		return Node4::TYPE_STRING;
	if (nb_starts)
	{
		if (nb_arguments == 1)
			return Node4::TYPE_UINT;
		else if (only_nb)
			return Node4::TYPE_UINT_VECTOR;
		else if (nb_arguments == 2)
			return Node4::TYPE_MAP_UINT_STRING;
		else
			return Node4::TYPE_MAP_UINT_STRING_VECTOR;
	}
	else
	{
		if (nb_arguments == 1)
			return Node4::TYPE_STRING;
		else
			return Node4::TYPE_STRING_VECTOR;
	}
	log.warn("Could not identify type for directive '" + node->value + "'.");
	return Node4::TYPE_STRING;
}

Node4::ValueType	typeToEnum_(std::string)
{
	return Node4::TYPE_STRING;
}

Node4::ValueType	typeToEnum_(unsigned int)
{
	return Node4::TYPE_UINT;
}

Node4::ValueType	typeToEnum_(std::vector<std::string>)
{
	return Node4::TYPE_STRING_VECTOR;
}

Node4::ValueType	typeToEnum_(std::vector<unsigned int>)
{
	return Node4::TYPE_UINT_VECTOR;
}

Node4::ValueType	typeToEnum_(std::map<unsigned int, std::string>)
{
	return Node4::TYPE_MAP_UINT_STRING;
}

Node4::ValueType	typeToEnum_(std::map<unsigned int, std::vector<std::string> >)
{
	return Node4::TYPE_MAP_UINT_STRING_VECTOR;
}

// #########################################################
};
};

