#ifndef NODE4_HPP
#define NODE4_HPP

#include <string>
#include "config/Lexer.hpp"
#include <vector>
#include <map>
#include "global.hpp"

namespace cfg
{
// #########################################################

class Node4
{
	private:
	/**
	 * Generate a quick representation of the current Node4, useful for debugging.
	 *
	 * @return A string representation of the node.
	 */
	str_t	fastStr_(void);

	public:
	/**
	 * The possible type for a directive from the server configuration.
	 */
	enum ValueType
	{
		/**
		 * #### `str_t`
		 */
		TYPE_STRING,
		/**
		 * #### `uint_t`
		 */
		TYPE_UINT,
		/**
		 * #### `vecstr_t`
		 */
		TYPE_STRING_VECTOR,
		/**
		 * #### `vecuint_t`
		 */
		TYPE_UINT_VECTOR,
		/**
		 * #### `mapstr_t`
		 */
		TYPE_MAP_UINT_STRING,
		/**
		 * #### `mapvec_t`
		 */
		TYPE_MAP_UINT_STRING_VECTOR,
	};
	/**
	 * The value of a directive (node4), containing its type and its data as a void pointer.
	 * The value data is allocated into the heap.
	 */
	class Value
	{
		public:
		/**
		 * The type of the value.
		 */
		ValueType	type;
		/**
		 * A pointer to the allocated data. Could be any type specified by the type property.
		 */
		void		*data;

		Value(void);
		~Value(void);

		/**
		 * Return the value with a specified type. The specified type MUST be the real type, everything is based
		 * on confidence.
		 *
		 * @tparam T The type of the data.
		 * @return A T pointer to the data.
		 */
		template <typename T>
		T		*getAs(void);

		/**
		 * Set the value with a specified type. The specified type MUST be the real type, everything is based
		 * on confidence. Be careful, the pointer is reallocated.
		 *
		 * @tparam T The type of the data.
		 * @param data_ The value of type T, this value will replace the current data_ content.
		 */
		template <typename T>
		void	setAs(T data_);

		/**
		 * Delete the data with the associated pointer.
		 */
		void	deleteData(void);
	};

	/**
	 * The name of the node4/directive. For example, if "listen 80;" is present into the config file, the node4 name
	 * will be "listen".
	 */
	str_t	name;

	/**
	 * The value of the node4, an instance of the Value class.
	 */
	Value		value;

	/**
	 * The pointer to the parent node4. If this is set, the this->parent->first_child node will be a pointer to this.
	 */
	Node4		*parent;

	/**
	 * If the node has children, the first one is pointed there.
	 */
	Node4		*first_child;

	/**
	 * The prev access to the current linked list (siblings).
	 */
	Node4		*prev_sibling;

	/**
	 * The next access to the current linked list (siblings).
	 */
	Node4		*next_sibling;

	Node4(ValueType type_id);
	~Node4(void);

	/**
	 * Generate a nice representation of the current node with its siblings and its children. Also display the parent
	 * reference. Add automatic tabulations based on children depth.
	 *
	 * @return A string containing the representation of the node and its links.
	 */
	str_t				toString(void);

	/**
	 * Iter on each child and its siblings and return a sequence of all the nodes whose name match with the child_name
	 * parameter.
	 *
	 * @param child_name The name of the children to look for. Can be unique or not.
	 * @return A sequence with node pointers.
	 */
	std::vector<Node4 *>	access(const str_t& child_name);
};

namespace n4u
{
	/**
	 * Takes two lexer token-node iterators and guess the delimited directive value type. For example, if it has some
	 * uint_ts, the return value will be TYPE_UINT_VECTOR.
	 *
	 * @param node The current token-node for the directive start.
	 * @param node The end token-node for the directive end; can be a delimiter, an end of file delimiter, or another
	 * directive, etc.
	 * @return The value type (an enum value).
	 */
	Node4::ValueType	dataType_(
		std::vector<Lexer::TokenNode>::const_iterator node,
		std::vector<Lexer::TokenNode>::const_iterator end
	);

	/**
	 * Create a node based on the requested type.
	 *
	 * @param type The type of the node to create. Can be any of the ValueType enum values.
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createNode4(Node4::ValueType type);

	/**
	 * Create a node with a string typed value.
	 *
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createStringNode4(void);

	/**
	 * Create a node with an uint_t typed value.
	 *
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createUintNode4(void);

	/**
	 * Create a node with a vector<string> typed value.
	 *
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createStringVectorNode4(void);

	/**
	 * Create a node with a vector<uinsigned int> typed value.
	 *
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createUintVectorNode4(void);

	/**
	 * Create a node with a map<uinsigned int, string> typed value.
	 *
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createMapUintStringNode4(void);

	/**
	 * Create a node with a map<uinsigned int, vector<string>> typed value.
	 *
	 * @return A pointer to the newly allocated node.
	 */
	Node4				*createMapUintStringVectorNode4(void);

	/**
	 * Take one of the sixth possible typed value, and return the corresponding enum value with a dispatcher.
	 *
	 * @return The type associated with the parameter.
	 */
	Node4::ValueType	typeToEnum_(str_t);

	/**
	 * Take one of the sixth possible typed value, and return the corresponding enum value with a dispatcher.
	 *
	 * @return The type associated with the parameter.
	 */
	Node4::ValueType	typeToEnum_(uint_t);

	/**
	 * Take one of the sixth possible typed value, and return the corresponding enum value with a dispatcher.
	 *
	 * @return The type associated with the parameter.
	 */
	Node4::ValueType	typeToEnum_(vecstr_t);

	/**
	 * Take one of the sixth possible typed value, and return the corresponding enum value with a dispatcher.
	 *
	 * @return The type associated with the parameter.
	 */
	Node4::ValueType	typeToEnum_(vecuint_t);

	/**
	 * Take one of the sixth possible typed value, and return the corresponding enum value with a dispatcher.
	 *
	 * @return The type associated with the parameter.
	 */
	Node4::ValueType	typeToEnum_(mapstr_t);

	/**
	 * Take one of the sixth possible typed value, and return the corresponding enum value with a dispatcher.
	 *
	 * @return The type associated with the parameter.
	 */
	Node4::ValueType	typeToEnum_(mapvec_t);
};

// #########################################################
};

#include "Node4.tpp"

#endif
