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
	 cfg::Parser		parser(argv[1]);
	 cfg::HttpConfig	conf;

	parser.parse();
	conf.generate(parser.getNodes());

	Directive<std::string>	http = conf.http();

	// try
	// {
	// 	Directive<std::string>	http = httpconf.http();

	// 	std::vector<Directive<$> >	servers = http.find<std::string>("server");
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// }



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

	std::string		str("Hello world!");
	try { cfg::magic_cast<std::string>(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	unsigned int	uintv = 42;
	try { cfg::magic_cast<std::string>(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::vector<std::string>	vector_str;
	vector_str.push_back("Hello");
	vector_str.push_back("world!");
	try { cfg::magic_cast<std::string>(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(vector_str); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::vector<unsigned int>	vector_uintv;
	vector_uintv.push_back(40);
	vector_uintv.push_back(2);
	try { cfg::magic_cast<std::string>(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(vector_uintv); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::map<unsigned int, std::string>	map1;
	map1.insert(std::make_pair(404, str));
	try { cfg::magic_cast<std::string>(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(map1); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	std::cout << std::endl;

	std::map<unsigned int, std::vector<std::string> >	map2;
	map2.insert(std::make_pair(404, vector_str));
	try { cfg::magic_cast<std::string>(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<unsigned int>(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<std::string> >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::vector<unsigned int> >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::string> >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

	try { cfg::magic_cast<std::map<unsigned int, std::vector<std::string> > >(map2); }
	catch (const std::exception& e) { std::cerr << RED "Conversion went wrong! Reason: " << e.what() << RESET << '\n'; }

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