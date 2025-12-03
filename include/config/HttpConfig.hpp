#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <vector>
#include <map>
#include <string>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Node4.hpp"

namespace Config
{
// #########################################################

class HttpConfig
{
	public:
	template <typename T>
	struct Directive
	{
		T		value;
		Node4	*node;
		Directive(T value_, Node4 *node_);
		~Directive(void);
	};

	HttpConfig(void);
	~HttpConfig(void);

	template <typename T>
	std::vector<Directive<T> >	get(const std::string& prop_name, const Node4 *parent = NULL);
	void						generate(const std::vector<Lexer::TokenNode>& nodes) throw (ParsingException);
	const Node4					*getRoot(void) const;

	private:
	template <typename T>
	struct GetData
	{
		int							depth;
		const Node4					*parent;
		Node4::ValueType			type;
		const std::string&			prop_name;
		std::vector<Directive<T> >	local_res;

		GetData(const std::string& prop_name_, const Node4 *parent_, unsigned int depth_);
		GetData(const GetData& other);
		~GetData(void);
	};

	Node4	*root_;

	template <typename T>
	std::vector<Directive<T> >	get_(GetData<T>& ddata);
};

// #########################################################
};

#include "HttpConfig.tpp"

#endif