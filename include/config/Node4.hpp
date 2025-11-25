#ifndef NODE4_HPP
#define NODE4_HPP

#include <string>
#include "config/Lexer.hpp"
#include <vector>
#include <map>

namespace Config
{
// #########################################################

class Node4
{
	private:
	std::string	fastStr(void);

	public:
	enum ValueType
	{
		TYPE_STRING,
		TYPE_UINT,
		TYPE_STRING_VECTOR,
		TYPE_UINT_VECTOR,
		TYPE_MAP_UINT_STRING,
		TYPE_MAP_UINT_STRING_VECTOR,
	};
	class Value
	{
		public:
		ValueType	type;
		void		*data;

		Value(void);
		~Value(void);

		template <typename T>
		T		*getAs(void);
		template <typename T>
		void	setAs(T data_);
		void						deleteData(void);
	};

	std::string	name;
	Value		value;
	Node4		*parent;
	Node4		*first_child;
	Node4		*prev_sibling;
	Node4		*next_sibling;

	Node4(ValueType type_id);
	~Node4(void);

	std::string				toString(void);
	std::vector<Node4 *>	access(const std::string& child_name);
	std::vector<Node4 *>	access(const std::string& child_name) const;
};

namespace Node4Utils
{
	Node4::ValueType	dataType_(
		std::vector<Lexer::TokenNode>::const_iterator node,
		std::vector<Lexer::TokenNode>::const_iterator end
	);
	Node4				*createNode4(Node4::ValueType type);
	Node4				*createStringNode4(void);
	Node4				*createUintNode4(void);
	Node4				*createStringVectorNode4(void);
	Node4				*createUintVectorNode4(void);
	Node4				*createMapUintStringNode4(void);
	Node4				*createMapUintStringVectorNode4(void);

	Node4::ValueType	typeToEnum_(std::string);
	Node4::ValueType	typeToEnum_(unsigned int);
	Node4::ValueType	typeToEnum_(std::vector<std::string>);
	Node4::ValueType	typeToEnum_(std::vector<unsigned int>);
	Node4::ValueType	typeToEnum_(std::map<unsigned int, std::string>);
	Node4::ValueType	typeToEnum_(std::map<unsigned int, std::vector<std::string> >);
};

// #########################################################
};

#include "Node4.tpp"

#endif
