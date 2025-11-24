#ifndef NODE4_HPP
#define NODE4_HPP

#include <string>

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
		TYPE_NULL,
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

		template <typename T> T		*getAs(void);
		template <typename T> void	setAs(T data_);
		void						deleteData(void);
	};

	Node4(Value::DataType type_id);
	~Node4(void);

	std::string				toString(void);
	std::vector<Node_ *>	access(const std::string& child_name);
	std::vector<Node_ *>	access(const std::string& child_name) const;
};

// #########################################################
};

#include "Node4.tpp"

#endif
