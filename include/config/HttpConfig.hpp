#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <vector>
#include <map>
#include <string>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Node4.hpp"
#include "types.hpp"

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
	std::vector<Directive<R> >	find(const std::string& prop_name);
};

template <typename T>
std::ostream	operator<<(std::ostream &os, const Directive<T>& directive);

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the string Directive.
 */
typedef Directive<std::string>											StrDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the unsigned int Directive.
 */
typedef Directive<unsigned int>											UintDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the vector<unsigned int> Directive.
 */
typedef Directive<std::vector<std::string> >							StrVecDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the vector<unsigned int> Directive.
 */
typedef Directive<std::vector<unsigned int> >							UintVecDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the map<unsigned int, string> Directive.
 */
typedef Directive<std::map<unsigned int, std::string> >					MapDirective;

/**
 * An alias type for a better reading and better usage in the middleware steps.
 * Represent the map<unsigned int, vector<string>> Directive.
 */
typedef Directive<std::map<unsigned int, std::vector<std::string> > >	MapVecDirective;

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
	std::vector<Directive<T> >	get(const std::string& prop_name, const Node4 *parent = NULL);

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
	std::vector<Directive<T> >	get(const std::string& prop_name, const Directive<P>& directive);

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
	Directive<std::string>		http(void);

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
		const Node4						*parent;
		/**
		 * The property name to look for.
		 */
		const std::string&				prop_name;
		/**
		 * The result of the search for the current reference.
		 */
		std::vector<Directive<T> >		local_res;

		GetData(const std::string& prop_name_, const Node4 *parent_);
		GetData(const GetData& other);
		~GetData(void);
	};

	/**
	 * The root of the configuration, also named as http().
	 */
	Node4	*root_;

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