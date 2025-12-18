#include <cstdlib>

#include "ServerInstance.hpp"
#include "utils.hpp"

ServerInstance::ServerInstance(StrDirective& server, uint32_t server_index)
	: server_(server), server_index_(server_index)
{

}

ServerInstance::~ServerInstance()
{
	
}

in_addr_t inet_addr_secure(const std::string& ip)
{
	in_addr addr;

	int ret = inet_pton(AF_INET, ip.c_str(), &addr);
	if (ret != 1)
		throw std::invalid_argument("invalid IPv4 address: " + ip);

	return addr.s_addr;
}

void ServerInstance::init()
{
	std::vector<std::string> listen_directives;
	
	try {
		listen_directives = server_.get<std::vector<std::string> >("listen");

		for (size_t j = 0; j < listen_directives.size(); j++)
		{
			size_t sep = listen_directives[j].find(":");

			if (sep == std::string::npos)
			{
				ListenProp prop;
				prop.ip = INADDR_ANY;
				prop.port = (uint)std::atoi(listen_directives[j].c_str());

				if (prop.port == 0) throw std::runtime_error("Unauthorized port 0 in configuration");

				listens_.push_back(prop);
			}
			else
			{
				std::string ip = listen_directives[j].substr(0, sep);
				if (ip == "localhost") ip = "127.0.0.1";

				ListenProp prop;
				prop.ip = inet_addr_secure(ip);
				prop.port = (uint)std::atoi(listen_directives[j].c_str() + sep + 1);

				if (prop.port == 0) throw std::runtime_error("Unauthorized port 0 in configuration");

				listens_.push_back(prop);
			}
		}
		// std::cout << cfg::util::represent(listen_directives) << std::endl;
	} catch (const std::exception& e) {
		throw std::runtime_error("Invalid listen value for server " + to_string(server_index_) + ". Error: " + e.what());
	}
	try {
		server_names_ = server_.get<std::vector<std::string> >("server_name");
		// std::cout << cfg::util::represent(server_names_) << std::endl;
	} catch (const std::exception& e) {
		throw std::runtime_error("Invalid server_name value for server " + to_string(server_index_) + ". Error: " + e.what());
	}
}