#include <iostream>
#include "config/Parser.hpp"
#include "config/HttpConfig.hpp"

int	main(int argc, char **argv)
{
	if (argc ^ 2)
	{
		std::cout << "Provide a unique configuration file." << std::endl;
		return 1;
	}
	Config::Parser		parser(argv[1]);
	Config::HttpConfig	httpconf;

	parser.parse();
	httpconf.generate(parser.getNodes());
}