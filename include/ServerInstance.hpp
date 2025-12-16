#ifndef __SERVERINSTANCE_HPP__
# define __SERVERINSTANCE_HPP__

#include <vector>
#include <string>
#include <arpa/inet.h>

struct ListenProp {
	in_addr_t ip;
	uint port;
};

class ServerInstance
{
private:
	std::vector<ListenProp> listen_;
	std::vector<std::string> server_names_;
	
public:
	ServerInstance();
	~ServerInstance();
};


#endif
