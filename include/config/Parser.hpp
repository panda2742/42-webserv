#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "types.hpp"
#include "HttpConfig.hpp"
#include "Lexer.hpp"

namespace Config
{
// #########################################################

class ParsingException: public std::runtime_error
{
	public:
	ParsingException(const std::string& msg);
};

class Parser
{
	private:
	HttpConfig						config_;
	std::string						filename_;
	std::string						raw_config_;
	std::vector<Lexer::TokenNode>	lexer_nodes_;

	void	saveRaw_(void) throw(ParsingException);
	void	tokenize_(void) throw(ParsingException);
	void	generateStruct_(void) throw(ParsingException);

	public:
	Parser(const std::string& filename);
	~Parser(void);

	HttpConfig&	parse(void) throw(ParsingException);
};

// #########################################################
};
#endif
