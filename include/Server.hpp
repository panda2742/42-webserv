#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <vector>
#include <cstdlib>
#include <map>
#include <stdint.h>
#include "http/HttpConnection.hpp"
#include "http/HttpResponse.hpp"
#include "ServerInstance.hpp"
#include "program.hpp"
#include "utils_structs.hpp"
#include "config/HttpConfig.hpp"

class Server
{

private:
	bool running_;
	bool is_child_;

	std::vector<int> listen_fd_;
	std::vector<FdContext> listen_context_;

	int epoll_fd_;

	cfg::HttpConfig& conf_;
	std::vector<ServerInstance> instances_;

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
	Server(cfg::HttpConfig &conf);
	~Server();

	void init();
	void run();
	void clean();
	
	int addCgiInFd(int fd, FdContext* fd_context);
	int addCgiOutFd(int fd, FdContext* fd_context);
	int removeCgiFd(int fd);

	void setChild() { is_child_ = true; }
};

#endif
