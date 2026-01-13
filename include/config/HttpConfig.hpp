#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <vector>
#include <map>
#include <string>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Node4.hpp"
#include "types.hpp"
#include "global.hpp"

/**
 * Represent a config directive with its associated value. This struct is an overlay to the Node4 struct, too
 * complicated to use outside of the parsing.
 * @tparam T The type of the value of the directive.
 */
template <typename T>
struct Directive
{
	/**
	 * The value of the directive.
	 */
	T			value;

	/**
	 * The associated Node4 pointer, because Directive is an overlay of Node4. Used for internal methods.
	 */
	 cfg::Node4	*node;

	Directive(T value_, cfg::Node4 *node_);
	Directive(const Directive& other);
	~Directive(void);

	/**
	 * Iter on each sibling of this first child and return all the Directive whose name is the same as prop_name.
	 * Useful if you want to get all the "server" nodes from a "http" element for example.
	 *
	 * @tparam R The type of each returned directive.
	 * @param prop_name The property to look for.
	 * @return The sequence of each Directive.
	 */
	template <typename R>
	std::vector<Directive<R> >	find(const str_t& prop_name);

	/**
	 * Iter on each sibling of this first child and build an instance of R. Each Directive is collapsed and merged into
	 * a unique value whose. The objective is to concatenate every possible value into a single vector/map.
	 * Please, BE ABSOLUTELY CAREFUL WHILE USING THIS and have a look at the convert table of the magic_cast.
	 *
	 * @tparam R The type of the final value.
	 * @param prop_name The property to look for.
	 * @return The built value (instance of R).
	 */
	template <typename R>
	R	get(const str_t& prop_name);
};

template <typename T>
std::ostream&	operator<<(std::ostream &os, const Directive<T>& directive);

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the string Directive.
 */
typedef Directive<str_t>		StrDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the uint_t Directive.
 */
typedef Directive<uint_t>		UintDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the vector<uint_t> Directive.
 */
typedef Directive<vecstr_t >	StrVecDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the vector<uint_t> Directive.
 */
typedef Directive<vecuint_t >	UintVecDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the map<uint_t, string> Directive.
 */
typedef Directive<mapstr_t >	MapDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the map<uint_t, vector<string>> Directive.
 */
typedef Directive<mapvec_t>		MapVecDirective;

namespace cfg
{
// #########################################################

/**
 * The main class that manages the server configuration. All directives/nodes are stored there.
 */
class HttpConfig
{
	public:
	HttpConfig(void);
	~HttpConfig(void);

	/**
	 * The get method takes a reference node as parent, and will return the most logical value based on nesting.
	 * First, it will look for every sibling of the reference, and if values are found, it will take those values as
	 * result.
	 * If after the first check, no value is found, it will look for a value that has been defined in an outside nest,
	 * such as parent or parent siblings.
	 * Then, every matching result is returned into a sequence of Directive.
	 *
	 * @tparam T The type of the Directives to get.
	 * @param prop_name The property to look for.
	 * @param parent The reference/parent node.
	 * @return The sequence of each directive.
	 */
	template <typename T>
	std::vector<Directive<T> >	get(const str_t& prop_name, const Node4 *parent = NULL);

	/**
	 * The get method takes a reference node as parent, and will return the most logical value based on nesting.
	 * First, it will look for every sibling of the reference, and if values are found, it will take those values as
	 * result.
	 * If after the first check, no value is found, it will look for a value that has been defined in an outside nest,
	 * such as parent or parent siblings.
	 * Then, every matching result is returned into a sequence of Directive.
	 *
	 * @tparam T The type of the Directives to get.
	 * @tparam P The type of the reference Directive passed as parameter.
	 * @param prop_name The property to look for.
	 * @param directive The reference/parent directive.
	 * @return The sequence of each directive.
	 */
	template <typename T, typename P>
	std::vector<Directive<T> >	get(const str_t& prop_name, const Directive<P>& directive);

	/**
	 * Takes the sequence of TokenNodes from the lexer and build the nested linked list.
	 *
	 * @param nodes The sequence of token nodes.
	 */
	void						generate(const std::vector<Lexer::TokenNode>& nodes) throw (ParsingException);

	/**
	 * Return the first node (the root) of the configuration as a Directive.
	 *
	 * @return The directive instance.
	 */
	Directive<str_t>			http(void);

	/**
	 * Return the boolean whether an error occurred during the parsing.
	 *
	 * @return The boolean of the test.
	 */
	bool	getErrorOccurred(void) const;

	private:
	/**
	 * This structure is used for storing data during the search.
	 *
	 * @tparam The type of the Directive values to get.
	 */
	template <typename T>
	struct GetData
	{
		/**
		 * The parent and current node to look for.
		 */
		const Node4					*parent;
		/**
		 * The property name to look for.
		 */
		const str_t&				prop_name;
		/**
		 * The result of the search for the current reference.
		 */
		std::vector<Directive<T> >	local_res;

		GetData(const str_t& prop_name_, const Node4 *parent_);
		GetData(const GetData& other);
		~GetData(void);
	};

	/**
	 * The root of the configuration, also named as http().
	 */
	Node4	*root_;
	/**
	 * If an error occurs during the parsing, this will be set to true.
	 */
	bool	error_occurred_;

	/**
	 * The core implementation of the get function. Only called by the HttpConfig::get() method.
	 *
	 * @tparam T The type of the Directives to get.
	 * @param ddata The GetData instance to store searching data.
	 * @return The sequence of directives.
	 */
	template <typename T>
	std::vector<Directive<T> >	get_(GetData<T>& ddata);
};

// #########################################################
};

#include "HttpConfig.tpp"

#endif