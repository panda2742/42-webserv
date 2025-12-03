#ifndef __UTILS_STRUCTS_HPP__
# define __UTILS_STRUCTS_HPP__

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
		HttpResponse* cgi_owner_response;
		int fd;
	};
};

#endif
