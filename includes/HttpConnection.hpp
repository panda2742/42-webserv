#ifndef __HTTP_CONNECTION_HPP__
# define __HTTP_CONNECTION_HPP__

#include <vector>
#include <iostream>
#include <deque>
#include <sys/types.h>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "utils_structs.hpp"

class Server;

class HttpConnection
{

private:
	int socket_fd_;
	FdContext context_;
	
	Server& server_;

	std::vector<char> raw_;
	bool header_;
	size_t content_size_;
	size_t header_size_;

	ssize_t find(const std::string& search, size_t range);
	std::string findHeaderContent(const std::string& key, size_t range);

	std::deque<HttpRequest> requests_;
	std::deque<HttpResponse> responses_;

	bool handleRequest();
	
public:
	HttpConnection(int socket_fd, Server& server);
	~HttpConnection();

	FdContext* getContext() { return &context_; }
	void receiveContent(char *content, size_t size);
	bool sendResponse();
	void clear();

};

#endif
