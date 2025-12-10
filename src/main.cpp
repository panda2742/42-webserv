#include <iostream>
#include "config/Parser.hpp"
#include "config/HttpConfig.hpp"
#include "config/MagicCast.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "tests.hpp"

int	main(int argc, char **argv)
{
	if (argc ^ 2)
	{
		std::cout << "Provide a unique configuration file." << std::endl;
		return 1;
	}
	 cfg::Parser		parser(argv[1]);
	 cfg::HttpConfig	conf;

	parser.parse();
	conf.generate(parser.getNodes());

	magic_cast_test();
	directive_get_test(conf);

	try {
		Server server;
		server.run();
	}
	catch (const std::exception& e) {
		Logger::error(e.what());
		// std::cerr << "Erreur: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}