#include "config/HttpConfig.hpp"

void	directive_get_test(cfg::HttpConfig& conf)
{
	StrDirective	http = conf.http();

	std::vector<StrDirective>	servers = http.find<std::string>("server");
}
