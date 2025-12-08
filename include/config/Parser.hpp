#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "types.hpp"
#include "Lexer.hpp"

namespace cfg
{
// #########################################################

class ParsingException: public std::runtime_error
{
	public:
	ParsingException(const std::string& msg)
		: std::runtime_error(RED PARSING_ERROR_MSG + msg + RESET) {}
};

class Parser
{
	private:
	std::string						filename_;
	std::string						raw_config_;
	std::vector<Lexer::TokenNode>	lexer_nodes_;

	void	saveRaw_(void) throw(ParsingException);
	void	tokenize_(void) throw(ParsingException);

	public:
	Parser(const std::string& filename)
		: filename_(filename) {}
	~Parser(void) {}

	const std::vector<Lexer::TokenNode>&	getNodes(void) const
	{
		return lexer_nodes_;
	}
	void	parse(void) throw(ParsingException);
};

// #########################################################
};
#endif
