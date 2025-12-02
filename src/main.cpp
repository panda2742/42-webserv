#include <iostream>
#include "config/Parser.hpp"
#include "config/HttpConfig.hpp"
#include "config/MagicCast.hpp"
#include "Logger.hpp"
#include "Server.hpp"

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

	// // Directive "root" donc http
	// Directive<std::string>					root = httpconf.http();
	// // Dans http, je recup toutes les directives server
	// std::vector<Directive<std::string> >	servers = root.find<std::string>("server");

	// for (std::vector<Directive<std::string> >::const_iterator it = servers.begin(); it != servers.end(); ++it)
	// {
	// 	// Getting each server
	// 	Directive<std::string>	server = (*it);
	// 	// Getting ports for each server
	// 	std::vector<Directive<unsigned int> >	ports = server.find<unsigned int>("listen");

	// 	// Displaying first port
	// 	std::cout << ports.size() << std::endl;
	// }

	try
	{
		std::string	hello_world = std::string("hello world");
		Config::magic_cast<unsigned int>(hello_world);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	try {
		Server server;
		server.run();
	}
	catch (const std::exception& e) {
		Logger::error(e.what());
		// std::cerr << "Erreur: " << e.what() << std::endl;
		return 1;
	}
}