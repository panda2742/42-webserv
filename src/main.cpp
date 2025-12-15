#include <iostream>
#include "config/Parser.hpp"
#include "config/HttpConfig.hpp"
#include "config/MagicCast.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "tests.hpp"
#include <signal.h>

void exit_signal(int sig);

int main(int argc, char **argv)
{
	if (argc ^ 2)
	{
		std::cout << "Provide a unique configuration file." << std::endl;
		return 1;
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
	return 0;
}