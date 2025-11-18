#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <vector>
#include <map>
#include <string>
#include "Lexer.hpp"

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

			void	deleteData(void);
		}		value;
		Node_	*parent;
		Node_	*first_child;
		Node_	*prev_sibling;
		Node_	*next_sibling;

		Node_(Value::DataType type_id)
			: parent(NULL), first_child(NULL), prev_sibling(NULL), next_sibling(NULL)
			{
				value.type = type_id;
				value.data = NULL;
			}

		~Node_(void);
	};

	static Node_	*createStringNode_(const std::string& str);
	static Node_	*createUintNode_(unsigned int value);
	static Node_	*createStringVectorNode_(void);
	static Node_	*createUintVectorNode_(void);
	static Node_	*createMapUintStringNode_(void);
	static Node_	*createMapUintStringVectorNode_(void);

	public:
	HttpConfig(void) {}
	~HttpConfig(void) {}
};


// #########################################################
};

#endif