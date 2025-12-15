#include "config/HttpConfig.hpp"
#include <iostream>

void	directive_get_test(cfg::HttpConfig& conf)
{
	StrDirective	http = conf.http();
	StrDirective		first_server = http.find<std::string>("server").at(0);

	std::cout << std::endl << std::endl;
	try {
		std::vector<unsigned int>	listens = first_server.get<std::vector<unsigned int> >("listen");
		std::cout << cfg::util::represent(listens) << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
