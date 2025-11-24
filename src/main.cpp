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

	const Config::HttpConfig::PublicNode_ 			*root = httpconf.getRoot();
	std::vector<Config::HttpConfig::PublicNode_ *>	servers = root->access("server");

	for (std::vector<Config::HttpConfig::PublicNode_ *>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::vector<Config::HttpConfig::Node_t<unsigned int> >	ports = httpconf.get<unsigned int>("listen", *it);

		std::cout << "For server " RED << &(*it) << RESET ", listening on ports: ";
		for (std::vector<Config::HttpConfig::Node_t<unsigned int> >::iterator jt = ports.begin(); jt != ports.end(); ++jt)
			std::cout << BLURPLE << (*jt).value << RESET << ", ";
		std::cout << std::endl;
	}
}