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

struct session_data
{
	time_t creation_time;
	size_t request_amount;
};

class ServerInstance
{
private:
	StrDirective server_;
	uint32_t server_index_;

	std::vector<ListenProp> listens_;
	std::vector<std::string> server_names_;
	bool is_default_;
	Location locations_;

	std::map<std::string, session_data> sessions_;

	static Location *global_loc_;

public:
	ServerInstance(StrDirective& server, uint32_t server_index);
	~ServerInstance();

	void init();

	const std::vector<ListenProp>& getListens() const { return listens_; }
	const std::vector<std::string>& getServerNames() const { return server_names_; }
	bool hasDefaultName() const { return is_default_; }

	Location&	getLocations(void) { return locations_; }

	static void setGlobalLocation(Location *loc) { global_loc_ = loc; }
	static const Location *getGlobalLocation() { return global_loc_; }
	static void freeGlobalLocation() { delete global_loc_; }
	
	std::map<std::string, session_data>& getSessions() { return sessions_; }
	session_data *getSession(const std::string& key);

};


#endif
