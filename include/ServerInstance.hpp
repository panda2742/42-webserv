#ifndef __SERVERINSTANCE_HPP__
# define __SERVERINSTANCE_HPP__

#include <vector>
#include <string>
#include <arpa/inet.h>

#include "config/HttpConfig.hpp"
#include "Location.hpp"

struct ListenProp {
	union {
		struct {
			in_addr_t ip;
			uint port;
		};
		uint64_t salade_tomate_oignons;
	};
};

inline bool operator<(const ListenProp &a, const ListenProp &b)
{
	if (a.port != b.port) return a.port < b.port;
	return a.ip < b.ip;
}

inline bool operator==(const ListenProp &a, const ListenProp &b)
{
	return a.salade_tomate_oignons == b.salade_tomate_oignons;
}

class ServerInstance
{
private:
	StrDirective server_;
	uint32_t server_index_;

	std::vector<ListenProp> listens_;
	std::vector<std::string> server_names_;
	bool is_default_;
	std::string root_;
	std::map<unsigned int, std::string> error_pages_;
	std::vector<Location> locations_;
	
public:
	ServerInstance(StrDirective& server, uint32_t server_index);
	~ServerInstance();

	void init();

	const std::vector<ListenProp>& getListens() const { return listens_; }
	const std::vector<std::string>& getServerNames() const { return server_names_; }
	bool hasDefaultName() const { return is_default_; }
	const std::string& getRoot() const { return root_; }
	const std::map<unsigned int, std::string>& getErrorPages() const { return error_pages_; }
	
};


#endif
