#ifndef __SERVER_HPP__
# define __SERVER_HPP__

#include <vector>
#include <cstdlib>
#include <map>
#include "HttpConnection.hpp"

class Server
{

private:
	bool running_;
	int listen_fd_;
	int epoll_fd_;

	std::map<int, HttpConnection> connections_;

	void handleClient(int fd);

public:
	Server();
	~Server() {}

	void run();
};

#endif
