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
	union {
		struct {
			HttpResponse *cgi_owner_response;
			int client_fd;
		};
		struct {
			int fd;
			in_addr ip;
		};
		struct {
			const std::vector<ServerInstance*> *server_instances;
			uint32_t fd_index;
			uint32_t port;
		};
	};
};

#endif
