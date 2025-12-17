#ifndef __UTILS_STRUCTS_HPP__
# define __UTILS_STRUCTS_HPP__

#include "ServerInstance.hpp"

class HttpResponse;

enum FdType
{
	LISTEN,
	CLIENT,
	CGI_IN,
	CGI_OUT
};

struct FdContext
{
	FdType type;
	HttpResponse* cgi_owner_response;
	int fd;
	std::vector<ServerInstance> server_instances;
};

#endif
