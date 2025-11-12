#ifndef WEBSERV_CONF_HPP
#define WEBSERV_CONF_HPP

#include <vector>
#include <string>
#include <map>

namespace Parsing
{
	typedef
	struct WebservConf
	{
		/**
		 * Represents a website or a vhost.
		 */
		typedef struct Server
		{
			std::vector<std::map<std::string, unsigned int>>	listen;
		};

		std::vector<Server>	server;
	};
}

#endif