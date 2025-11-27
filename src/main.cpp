#include "Server.hpp"
#include "Logger.hpp"
#include <iostream>
#include <signal.h>

void exit_signal(int sig);

int main()
{
	try {
		struct sigaction	sigint_sa;
		sigint_sa.sa_flags = 0;
		sigint_sa.sa_handler = exit_signal;
		sigemptyset(&sigint_sa.sa_mask);
		sigaction(SIGINT, &sigint_sa, NULL);
		
		Server server;
		server.run();
	}
	catch (const std::exception& e) {
		Logger::error(e.what());
		// std::cerr << "Erreur: " << e.what() << std::endl;
		return 1;
	}
}