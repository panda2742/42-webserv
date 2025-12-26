#include <iostream>
#include "config/Parser.hpp"
#include "config/HttpConfig.hpp"
#include "config/MagicCast.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "tests.hpp"
#include "middleware/Middleware.hpp"
#include <signal.h>

void exit_signal(int sig);

int main(int argc, char **argv)
{
	if (argc ^ 2)
	{
		std::cout << "Provide a unique configuration file." << std::endl;
		return 1;
	}

	// Create the configuration class
	cfg::HttpConfig	conf;
	
	try
	{
		// Create a parser
		cfg::Parser	parser(argv[1]);
		// Parse the file
		parser.parse();
		
		// Load the configuration by passing parser lexer nodes
		conf.generate(parser.getNodes());

		// Call a new middleware on the config
		mdw::Middleware	middleware(conf);

		// Check if the configuration is valid.
		middleware.checkFormat(parser.getNodes());
	}
	catch (const std::exception& e)
	{
		std::cerr << "Middleware went wrong! Reason: " << e.what() << std::endl;
	}

	try
	{
		struct sigaction	sigint_sa;
		sigint_sa.sa_flags = 0;
		sigint_sa.sa_handler = exit_signal;
		sigemptyset(&sigint_sa.sa_mask);
		sigaction(SIGINT, &sigint_sa, NULL);

		Server server(conf);
		server.init();
		server.run();
	}
	catch (const std::exception& e)
	{
		Logger::error(e.what());
		return 1;
	}
	return 0;
}