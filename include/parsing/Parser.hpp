#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "types.hpp"

namespace Parsing
{
	struct WebservConf
	{

		struct Server
		{
			typedef unsigned int client_max_body_size_t;

			std::vector<std::map<std::string, unsigned int> >	listen;
			std::vector<std::string>							server_name;
			std::map<unsigned short int, std::string>			error_page;
			client_max_body_size_t								client_max_body_size;

			struct Location
			{

				std::string									root;
				std::vector<HttpTypes::Method>				methods;
				std::vector<std::string>					index;
				HttpTypes::ToggleValue						autoindex;
				std::string									upload_path;
				std::map<std::string, std::string>			cgi;
				std::map<unsigned short int, std::string>	redirect;
			};

			std::map<std::string, Location>	location;
		};

		std::vector<Server>	server;
	};

	namespace AST
	{
		enum Token
		{
			TokenSymbolOpen,
			TokenSymbolClose,
			TokenDelimiter,
			TokenDirective,
			TokenArgument
		};

		struct ASTNode
		{
			Token		type;
			std::string	value;

			ASTNode(const Token type, const std::string& value);
		};
	};

	class ParsingException: public std::runtime_error
	{
		public:
		ParsingException(const std::string& msg);
	};

	class Parser
	{
		typedef std::vector<AST::ASTNode>	ASTNodes;

		private:
		WebservConf	config_;
		std::string	filename_;
		std::string	raw_config_;
		ASTNodes	ast_nodes_;

		void	saveRaw_(void) throw(ParsingException);
		void	tokenize_(void) throw(ParsingException);

		static bool	isInCharset(char c, const char *cstr);
		static void	printTokens_(const ASTNodes& nodes);

		public:

		Parser(const std::string& filename);
		~Parser(void);

		WebservConf&	parse(void) throw(ParsingException);
	};
}

#endif
