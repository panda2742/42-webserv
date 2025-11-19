#ifndef __HTTP_CONNECTION_HPP__
# define __HTTP_CONNECTION_HPP__

#include <vector>
#include <iostream>
#include <deque>
#include <sys/types.h>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpConnection
{

private:
	int socket_fd_;

	std::vector<char> raw_;
	bool header_;
	size_t content_size_;
	size_t header_size_;

	ssize_t find(const std::string& search, size_t range);
	std::string findHeaderContent(const std::string& key, size_t range);

	std::deque<HttpRequest> requests_;
	std::deque<HttpResponse> responses_;

	void handleRequest();
	
public:
	HttpConnection(int socket_fd);
	~HttpConnection();

	void receiveContent(char *content, size_t size);
	void sendResponse();
	void clear();

};

#endif
