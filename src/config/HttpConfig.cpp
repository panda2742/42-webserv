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

HttpConfig::Node_	*HttpConfig::createNode(Node_::Value::DataType type)
{
	try
	{
		switch (type)
		{
			case Node_::Value::TYPE_NULL:
				return createNullNode_();
				break;
			case Node_::Value::TYPE_STRING:
				return createStringNode_();
				break;
			case Node_::Value::TYPE_UINT:
				return createUintNode_();
				break;
			case Node_::Value::TYPE_STRING_VECTOR:
				return createStringVectorNode_();
				break;
			case Node_::Value::TYPE_UINT_VECTOR:
				return createUintVectorNode_();
				break;
			case Node_::Value::TYPE_MAP_UINT_STRING:
				return createMapUintStringNode_();
				break;
			case Node_::Value::TYPE_MAP_UINT_STRING_VECTOR:
				return createMapUintStringVectorNode_();
				break;
			default:
				return NULL;
				break;
		}
	}
	catch (const std::bad_alloc& e)
	{
		log.error("Memory allocation failed in createNode: " + std::string(e.what()));
		return NULL;
	}
	return NULL;
}

HttpConfig::Node_	*HttpConfig::createNullNode_(void)
{
	Node_	*node = new Node_(Node_::Value::TYPE_NULL);
	node->value.data = NULL;
	return node;
}

HttpConfig::Node_	*HttpConfig::createStringNode_()
{
	Node_	*node = new Node_(Node_::Value::TYPE_STRING);
	node->value.data = new std::string();
	return node;
}

HttpConfig::Node_	*HttpConfig::createUintNode_()
{
	Node_	*node = new Node_(Node_::Value::TYPE_UINT);
	node->value.data = new unsigned int();
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
	Node_	*parent = root_;
	for (std::vector<Lexer::TokenNode>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (it->type == Lexer::TokenParent || it->type == Lexer::TokenDirective)
		{
			Node_::Value::DataType	type = dataType_(it, nodes.end());
			Node_					*node = createNode(type);

			node->parent = parent;

			if (parent->first_child == NULL)
				parent->first_child = node;
			else
			{
				Node_	*last = parent->first_child;
				while (last->next_sibling != NULL)
					last = last->next_sibling;
				last->next_sibling = node;
				node->prev_sibling = last;
			}


            std::vector<Lexer::TokenNode>::const_iterator current = it;
            bool shouldBreak = false;

            while (current != nodes.end() && !shouldBreak)
            {
                switch (current->type) {
                    case Lexer::TokenSymbolClose:
                        if (parent->parent == NULL) {
                            log.error("Unexpected token '}'. Cannot unfold current element.");
                            delete node;
                            return;
                        }
                        parent = parent->parent;
                        shouldBreak = true;
                        break;

                    case Lexer::TokenSymbolOpen:
                        parent = node;
                        shouldBreak = true;
                        break;

                    case Lexer::TokenDelimiter:
                        shouldBreak = true;
                        break;

                    default:
                        break;
                }

                if (!shouldBreak) {
                    ++current;
                    if (current == nodes.end())
						break;
                }
            }

            if (current > it)
			{
				if (current == nodes.end())
				{
					it = nodes.end();
					break;
				}
                it = current;
				--it;
			}
		}
	}
}

// #########################################################
};
