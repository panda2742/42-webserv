#include "Server.hpp"
#include "Logger.hpp"
#include <iostream>

int main()
{
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