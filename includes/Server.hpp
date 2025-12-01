#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <vector>
#include <cstdlib>
#include <map>
#include <stdint.h>
#include "HttpConnection.hpp"
#include "HttpResponse.hpp"
#include "program.hpp"
#include "utils_structs.hpp"

class Server
{

private:
	bool running_;
	int listen_fd_;
	FdContext listen_context_;
	int epoll_fd_;

	// std::map<int, FdContext> pipe_context_;
	std::map<int, HttpConnection> connections_;

	void handleCGI(struct epoll_event &epoll);
	void handleClient(struct epoll_event &epoll);
	void handleClientIN(int fd);
	void handleClientOUT(int fd);

	int removeFdEpoll(int fd);
	int addFdEpoll(int fd, uint32_t flags, FdContext* context);
	void removeClient(int fd, Logger::Level lvl);

public:
	Server();
	~Server();

	void run();

	int addCgiInFd(int fd, FdContext* fd_context);
	int addCgiOutFd(int fd, FdContext* fd_context);
	int removeCgiFd(int fd);
};

#endif
