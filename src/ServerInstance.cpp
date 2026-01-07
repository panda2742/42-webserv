#include <cstdlib>

#include "ServerInstance.hpp"
#include "utils.hpp"

ServerInstance::ServerInstance(StrDirective& server, uint32_t server_index)
	: server_(server), server_index_(server_index), locations_(server, NULL)
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

				if (prop.port == 0) throw std::invalid_argument("Unauthorized port 0 in configuration");

				listens_.push_back(prop);
			}
			else
			{
				std::string ip = listen_directives[j].substr(0, sep);
				if (ip == "localhost") ip = "127.0.0.1";

				ListenProp prop;
				prop.ip = inet_addr_secure(ip);
				prop.port = (uint)std::atoi(listen_directives[j].c_str() + sep + 1);

				if (listen_directives[j].size() - sep > 7 || prop.port != static_cast<unsigned short>(prop.port)) throw std::invalid_argument("Invalid port: " + std::string(listen_directives[j].c_str() + sep + 1) + " " + to_string(prop.port));
				if (prop.port == 0) throw std::invalid_argument("Unauthorized port 0 in configuration");

				listens_.push_back(prop);
			}
		}
		// std::cout << cfg::util::represent(listen_directives) << std::endl;
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid listen value for server " + to_string(server_index_) + ". Error: " + e.what());
	}
	try {
		is_default_ = false;
		server_names_ = server_.get<std::vector<std::string> >("server_name");

		for (size_t i = 0; i < server_names_.size(); i++)
		{
			if (server_names_[i] == "default")
			{
				is_default_ = true;
				break ;
			}
		}

		if (server_names_.size() == 0) is_default_ = true;

		// std::cout << cfg::util::represent(server_names_) << std::endl;
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid server_name value for server " + to_string(server_index_) + ". Error: " + e.what());
	}

	try {
		root_ = server_.find<std::string>("root").at(0).value;

		if (root_.empty()) throw std::invalid_argument("root is required");

		if (root_.size() - 1 == '/') root_.erase(root_.end() - 1);

	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid root value for server " + to_string(server_index_) + ". Error: " + e.what());
	}

	try {
		error_pages_ = server_.get<std::map<unsigned int, std::string> >("error_page");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid error_page value for server " + to_string(server_index_) + ". Error: " + e.what());
	}

	try {
		locations_.init();
		// std::vector<StrDirective> locations = server_.find<std::string>("location");

		// for (std::vector<StrDirective>::iterator it = locations.begin(); it != locations.end(); ++it)
		// {
		// 	locations_.push_back(Location(*it, NULL));
		// 	Location& loc = locations_.back();
		// 	loc.init();
		// }
		locations_.print();
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid locations value for server " + to_string(server_index_) + (server_.value.length() > 0 ? " " + server_.value : "") + ". Error: " + e.what());
	}
}