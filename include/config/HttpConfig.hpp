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

		std::string				fastStr(void);
		std::string				toString(void);
		std::vector<Node_ *>	access(const std::string& child_name);
		std::vector<Node_ *>	access(const std::string& child_name) const;
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

	static Node_::Value::DataType	typeToEnum_(std::string) { return Node_::Value::TYPE_STRING; };
	static Node_::Value::DataType	typeToEnum_(unsigned int) { return Node_::Value::TYPE_UINT; };
	static Node_::Value::DataType	typeToEnum_(std::vector<std::string>) { return Node_::Value::TYPE_STRING_VECTOR; };
	static Node_::Value::DataType	typeToEnum_(std::vector<unsigned int>) { return Node_::Value::TYPE_UINT_VECTOR; };
	static Node_::Value::DataType	typeToEnum_(std::map<unsigned int, std::string>) { return Node_::Value::TYPE_MAP_UINT_STRING; };
	static Node_::Value::DataType	typeToEnum_(std::map<unsigned int, std::vector<std::string> >) { return Node_::Value::TYPE_MAP_UINT_STRING_VECTOR; };

	public:
	typedef Node_	PublicNode_;
	template <typename T>
	struct Node_t
	{
		T		value;
		Node_	*node_parent;
		Node_t(T value_, Node_ *node_parent_): value(value_), node_parent(node_parent_) {}
		~Node_t(void) {}
	};
	HttpConfig(void): root_(createNullNode_())
	{
		root_->name = "http";
	}
	~HttpConfig(void)
	{
		delete root_;
	}

	void	generate(const std::vector<Lexer::TokenNode>& nodes) throw (ParsingException);
	template <typename T>
	std::vector<Node_t<T> >	get(const std::string& prop_name, const Node_ *parent = NULL);
	const Node_	*getRoot(void) const { return root_; }

	private:
	template <typename T>
	struct DepthData_
	{
		const std::string&		prop_name;
		const Node_				*parent;
		std::vector<Node_t<T> >	local_res;
		int						depth;
		Node_::Value::DataType	type;
		DepthData_(const std::string& prop_name_, const Node_ *parent_, unsigned int depth_)
			: prop_name(prop_name_), parent(parent_), depth(depth_) { type = typeToEnum_(T()); }
		DepthData_(const DepthData_& other)
			: prop_name(other.prop_name), parent(other.parent), local_res(other.local_res), depth(other.depth), type(other.type) {}
		~DepthData_(void) {}
	};
	template <typename T>
	std::vector<Node_t<T> >	get_(DepthData_<T>& ddata);
};

// #########################################################
};

#include "HttpConfig.tpp"

#endif