#ifndef __SERVERINSTANCE_HPP__
# define __SERVERINSTANCE_HPP__

#include <vector>
#include <string>

class ServerInstance
{
private:
	std::vector<unsigned int> ports_;
	std::vector<std::string> server_names_;
	
public:
	ServerInstance();
	~ServerInstance();
};


#endif
