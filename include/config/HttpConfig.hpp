#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <vector>
#include <map>
#include <string>
#include "Lexer.hpp"
#include "Parser.hpp"

namespace Config
{
// #########################################################

class HttpConfig
{
	private:
	struct Node_
	{
		struct Value
		{
			enum DataType
			{
				TYPE_NULL,
				TYPE_STRING,
				TYPE_UINT,
				TYPE_STRING_VECTOR,
				TYPE_UINT_VECTOR,
				TYPE_MAP_UINT_STRING,
				TYPE_MAP_UINT_STRING_VECTOR,
			}		type;
			void	*data;

			template <typename T>
			T		*getAs(void)
			{
				return static_cast<T *>(data);
			}
			template <typename T>
			void	setAs(T data_)
			{
				if (data)
					deleteData();
				data = new T(data_);
			}
			void	deleteData(void);
		}		value;
		std::string	name;
		Node_		*parent;
		Node_		*first_child;
		Node_		*prev_sibling;
		Node_		*next_sibling;

		Node_(Value::DataType type_id)
			: parent(NULL), first_child(NULL), prev_sibling(NULL), next_sibling(NULL)
			{
				value.type = type_id;
				value.data = NULL;
			}

		~Node_(void);

		std::string	fastStr(void);
		std::string	toString(unsigned int tabs);
	};

	Node_	*root_;

	static Node_::Value::DataType	dataType_(std::vector<Lexer::TokenNode>::const_iterator node, std::vector<Lexer::TokenNode>::const_iterator end);
	static Node_					*createNode(Node_::Value::DataType type);
	static Node_					*createNullNode_(void);
	static Node_					*createStringNode_(void);
	static Node_					*createUintNode_(void);
	static Node_					*createStringVectorNode_(void);
	static Node_					*createUintVectorNode_(void);
	static Node_					*createMapUintStringNode_(void);
	static Node_					*createMapUintStringVectorNode_(void);

	public:
	HttpConfig(void): root_(createStringNode_())
	{
		root_->value.setAs<std::string>("http");
	}
	~HttpConfig(void)
	{
		delete root_;
	}

	void	generate(const std::vector<Lexer::TokenNode>& nodes) throw (ParsingException);
};


// #########################################################
};

#endif