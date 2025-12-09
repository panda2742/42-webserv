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

/**
 * Represent an exception during the parsing. The message is specified into the constructor.
 */
class ParsingException: public std::runtime_error
{
	public:
	ParsingException(const std::string& msg)
		: std::runtime_error(PARSING_ERROR_MSG + msg) {}
};

/**
 * The class that handles the parsing of the configuration file.
 */
class Parser
{
	private:
	/**
	 * The name of the configuration file.
	 */
	std::string						filename_;

	/**
	 * The raw content of the configuration file. Everything is stored there.
	 */
	std::string						raw_config_;

	/**
	 * The list of nodes, in the same order they appear in the configuration file.
	 */
	std::vector<Lexer::TokenNode>	lexer_nodes_;

	/**
	 * This function open the file and save its content into the raw private attribute. The function also removes every
	 * comments and ignores it.
	 */
	void	saveRaw_(void) throw(ParsingException);

	/**
	 * Read each line of the configuration file saved in the raw, and generates the token based on the content of each
	 * word.
	 */
	void	tokenize_(void) throw(ParsingException);

	public:
	/**
	 * @param filename The name of the configuration file.
	 */
	Parser(const std::string& filename)
		: filename_(filename) {}
	~Parser(void) {}

	/**
	 * Return the sequence of token-nodes stored into the class. This is a const reference and not a copy.
	 *
	 * @return The reference to the stored list of token-nodes.
	 */
	const std::vector<Lexer::TokenNode>&	getNodes(void) const;

	/**
	 * Parse the configuration file and generate the token-nodes list. It also throws exceptions if problems occurs on
	 * file parsing. There is no much errors since the types of each directives are all checked in the middleware.
	 */
	void	parse(void) throw(ParsingException);
};

// #########################################################
};
#endif
