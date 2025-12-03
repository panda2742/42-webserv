#include <iostream>
#include "config/Parser.hpp"
#include "config/HttpConfig.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include <signal.h>

void exit_signal(int sig);

int main(int argc, char **argv)
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

	const Config::Node4				*root = httpconf.getRoot();
	std::vector<Config::Node4 *>	servers = root->access("server");

	for (std::vector<Config::Node4 *>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::vector<Config::HttpConfig::Directive<unsigned int> >	ports = httpconf.get<unsigned int>("listen", *it);

		std::cout << "For server " RED << &(*it) << RESET ", listening on ports: ";
		for (std::vector<Config::HttpConfig::Directive<unsigned int> >::iterator jt = ports.begin(); jt != ports.end(); ++jt)
			std::cout << BLURPLE << (*jt).value << RESET << ", ";
		std::cout << std::endl;
	}
	
	try
	{
		struct sigaction	sigint_sa;
		sigint_sa.sa_flags = 0;
		sigint_sa.sa_handler = exit_signal;
		sigemptyset(&sigint_sa.sa_mask);
		sigaction(SIGINT, &sigint_sa, NULL);
		
		Server server;
		server.init();
		server.run();
	}
	catch (const std::exception& e)
	{
		Logger::error(e.what());
		return 1;
	}

}