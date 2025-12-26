#include "config/HttpConfig.hpp"
#include <iostream>
#include "config/ConfigLogger.hpp"
#include "config/util.hpp"
#include <cstdlib>
#include <sstream>
#include "config/Node4.hpp"

namespace cfg
{
// #########################################################

HttpConfig::HttpConfig(void): root_(n4u::createStringNode4())
{
	root_->name = "http";
}

HttpConfig::~HttpConfig(void)
{
	delete root_;
}

Directive<str_t>	HttpConfig::http(void)
{
	return Directive<str_t>(*root_->value.getAs<str_t>(), root_);
}

void	HttpConfig::generate(const std::vector<Lexer::TokenNode>& nodes) throw(ParsingException)
{
	Node4	*parent = root_;
	for (std::vector<Lexer::TokenNode>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if (it->type == Lexer::TokenParent || it->type == Lexer::TokenDirective)
		{
			Node4::ValueType	type = n4u::dataType_(it, nodes.end());
			Node4					*node = n4u::createNode4(type);

			node->name = it->value;
			node->parent = parent;

			if (parent->first_child == NULL)
				parent->first_child = node;
			else
			{
				Node4	*last = parent->first_child;
				while (last->next_sibling != NULL)
					last = last->next_sibling;
				last->next_sibling = node;
				node->prev_sibling = last;
			}

            std::vector<Lexer::TokenNode>::const_iterator current = ++it;
            bool shouldBreak = false;
			size_t	i = 0;

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

				if (current->type == Lexer::TokenArgument && !shouldBreak)
				{
					switch (type)
					{
						case Node4::TYPE_STRING:
							node->value.setAs<str_t>(current->value);
							break;
						case Node4::TYPE_UINT:
							node->value.setAs<uint_t>(std::atoi(current->value.c_str()));
							break;
						case Node4::TYPE_STRING_VECTOR:
							if (i)
								node->value.getAs<vecstr_t >()->push_back(current->value);
							else
								node->value.setAs<vecstr_t >(vecstr_t(1, current->value));
							break;
						case Node4::TYPE_UINT_VECTOR:
							if (i)
								node->value.getAs<vecuint_t >()->push_back(std::atol(current->value.c_str()));
							else
								node->value.setAs<vecuint_t >(vecuint_t(1, std::atoi(current->value.c_str())));
							break;
						case Node4::TYPE_MAP_UINT_STRING:
							if (i)
							{
								mapstr_t    *m = node->value.getAs<mapstr_t >();
								m->begin()->second = current->value;
							}
							else
							{
								mapstr_t    m;
								m.insert(std::make_pair(std::atoi(current->value.c_str()), ""));
								node->value.setAs<mapstr_t >(m);
							}
							break;
						case Node4::TYPE_MAP_UINT_STRING_VECTOR:
							if (i)
							{
								mapvec_t    *m = node->value.getAs<mapvec_t>();
								m->begin()->second.push_back(current->value);
							}
							else
							{
								mapvec_t   m;
								m.insert(std::make_pair(std::atoi(current->value.c_str()), vecstr_t()));
								node->value.setAs<mapvec_t>(m);
							}
							break;
						default:
							break;
					}

					++current;
					if (current == nodes.end())
						break;
					++i;
				}
				else
				{
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
		else if (it->type == Lexer::TokenSymbolClose)
		{
			if (parent->parent == NULL) {
				log.error("Unexpected token '}'. Cannot unfold current element.");
				return;
			}
			parent = parent->parent;
		}
	}
}

// #########################################################
};
