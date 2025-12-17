#include "ServerInstance.hpp"

ServerInstance::ServerInstance(std::vector<ListenProp>& listen, std::vector<std::string>& server_names)
	: listens_(listen), server_names_(server_names)
{

}

ServerInstance::~ServerInstance()
{
	
}