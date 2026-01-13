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
	std::map<ListenProp, std::vector<ServerInstance*> > server_instance_map_;

	// std::map<int, FdContext> pipe_context_;
	std::map<int, HttpConnection> connections_;

	std::map<int, HttpResponse*>cgi_monitoring_;
	

	void handleCGI(struct epoll_event &epoll);
	void handleClient(struct epoll_event &epoll);
	void handleClientIN(int fd);
	void handleClientOUT(int fd);

	int removeFdEpoll(int fd);
	int addFdEpoll(int fd, uint32_t flags, FdContext* context);
	void removeClient(int fd, Logger::Level lvl);

	void monitorCGI();

public:
	Server(cfg::HttpConfig &conf);
	~Server();

	void init();
	void initInstances();
	void initSockets();
	void run();
	void clean();
	
	int addCgiInFd(int fd, FdContext* fd_context);
	int addCgiOutFd(int fd, FdContext* fd_context);
	int removeCgiFd(int fd);

	void setChild() { is_child_ = true; }

	void addMonitoredCGI(int pid, HttpResponse *resp) { cgi_monitoring_[pid] = resp; }
	void removeMonitoredCGI(int pid) { std::map<int, HttpResponse*>::iterator it = cgi_monitoring_.find(pid); if (it != cgi_monitoring_.end()) cgi_monitoring_.erase(it); }
};

#endif
