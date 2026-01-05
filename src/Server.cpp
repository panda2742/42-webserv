
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include "ServerInstance.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include "utils.hpp"

// #define BASEPORT 8080
#define MAX_EVENTS 64

bool siginted = false;

void exit_signal(int sig)
{
	if (sig == SIGINT)
		siginted = true;
}

int Server::removeFdEpoll(int fd)
{
	struct epoll_event ev;
	ev.data.ptr = NULL;
	return epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev);
}

int Server::addFdEpoll(int fd, uint32_t flags, FdContext* context)
{
	if (!context) return -1;
	struct epoll_event client_ev;
	client_ev.events = flags;
	client_ev.data.ptr = context;
	return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &client_ev);
}

void Server::removeClient(int fd, Logger::Level lvl)
{
	Logger::log(lvl, "Disconnect client (fd: " + to_string(fd) + std::string(")"));
	removeFdEpoll(fd);
	close(fd);
	std::map<int, HttpConnection>::iterator it = connections_.find(fd);
	if (it != connections_.end())
	{
		it->second.clear();
		connections_.erase(it);
	}
}

void Server::handleClientIN(int fd)
{
	std::vector<char> request_buffer;
	char buf[4096];
	ssize_t r;

	size_t size = 0;

	while ((r = recv(fd, buf, sizeof(buf), 0)) > 0) // TODO remove cette boucle et passer en un recv par call de handleClient
	{
		request_buffer.insert(request_buffer.end(), buf, buf + r);
		size += r;
	}

	if (r == 0) return removeClient(fd, Logger::INFO);

	std::map<int, HttpConnection>::iterator it = connections_.find(fd);
	if (it != connections_.end())
	{
		it->second.receiveContent(request_buffer.data(), size);
	}
	else
	{
		Logger::warn("received data for unknown fd (fd: " + to_string(fd) + std::string(")"));
	}
}

void Server::handleClientOUT(int fd)
{
	std::map<int, HttpConnection>::iterator it = connections_.find(fd);
	if (it == connections_.end())
		return;

	if (!it->second.sendResponse())
		removeClient(fd, Logger::WARN);
}

void Server::handleClient(struct epoll_event &epoll)
{
	FdContext* ctx = static_cast<FdContext*>(epoll.data.ptr);
	if (!ctx) return;

	int fd = ctx->fd;

	if (epoll.events & EPOLLIN)
	{
		handleClientIN(fd);
	}

	if (epoll.events & EPOLLOUT)
	{
		handleClientOUT(fd);
	}
}

void Server::handleCGI(struct epoll_event &epoll)
{
	FdContext* fd_context = static_cast<FdContext*>(epoll.data.ptr);
	HttpResponse* res = static_cast<HttpResponse*>(fd_context->cgi_owner_response);

	if (fd_context->type == CGI_IN)
	{
		res->sendBodyCGI();
	}
	else if (fd_context->type == CGI_OUT)
	{
		res->getContentCGI();
	}
}

int Server::addCgiInFd(int fd, FdContext* fd_context)
{
	if (addFdEpoll(fd, EPOLLOUT, fd_context) < 0)
	{
		Logger::error("cant connect the CGI to epoll");
		return -1;
	}
	return 0;
}

int Server::addCgiOutFd(int fd, FdContext* fd_context)
{
	if (addFdEpoll(fd, EPOLLIN, fd_context) < 0)
	{
		Logger::error("cant connect the CGI to epoll");
		return -1;
	}
	return 0;
}

int Server::removeCgiFd(int fd)
{
	return removeFdEpoll(fd);
}

Server::Server(cfg::HttpConfig &conf)
	: running_(false), is_child_(false), conf_(conf)
{
	// listen_fd_ = -1;
	epoll_fd_ = -1;
}

void Server::initInstances()
{
	StrDirective http = conf_.http();
	std::vector<StrDirective> servers = http.find<std::string>("server");

	for (size_t i = 0; i < servers.size(); i++)
	{
		instances_.push_back(ServerInstance(servers[i], i));
		instances_.back().init();
	}

	for (size_t i = 0; i < instances_.size(); i++)
	{
		ServerInstance &instance = instances_[i];
		const std::vector<ListenProp> &instance_listen_props = instance.getListens();

		for (size_t j = 0; j < instance_listen_props.size(); j++)
		{
			const std::vector<ServerInstance*> &server_instances = server_instance_map_[instance_listen_props[j]];
			if (std::find(server_instances.begin(), server_instances.end(), &instance) == server_instances.end())
			{
				server_instance_map_[instance_listen_props[j]].push_back(&instance);
			}
		}
	}
}

void Server::initSockets()
{
	epoll_fd_ = epoll_create(1);
	if (epoll_fd_ < 0)
		throw std::runtime_error("epoll_create failed");

	size_t total_listens = server_instance_map_.size();
	listen_context_.reserve(total_listens);
	listen_fd_.reserve(total_listens);

	for (
		std::map<ListenProp, std::vector<ServerInstance*> >::const_iterator it = server_instance_map_.begin();
		it != server_instance_map_.end();
		++it
	) {
		int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

		int opt = 1;
		setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		struct sockaddr_in addr;
		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(it->first.port);
		addr.sin_addr.s_addr = it->first.ip;

		if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			close(listen_fd);
			throw std::runtime_error("Socket binding (" + to_string(it->first.port) + "): " + std::string(strerror(errno)));
		}

		if (listen(listen_fd, SOMAXCONN) < 0)
		{
			close(listen_fd);
			throw std::runtime_error("listen failed");
		}

		fcntl(listen_fd, F_SETFL, O_NONBLOCK);

		FdContext listen_context;
		
		listen_context.type = LISTEN;
		listen_context.fd_index = static_cast<uint32_t>(listen_context_.size());
		listen_context.server_instances = &it->second;
		listen_context.port = it->first.port;

		listen_fd_.push_back(listen_fd);
		listen_context_.push_back(listen_context);

		if (addFdEpoll(listen_fd, EPOLLIN, &listen_context_[listen_context_.size() - 1]) < 0)
		{
			close(listen_fd);
			throw std::runtime_error("epoll to listen fd link failed");
		}

		in_addr addr_log;
		addr_log.s_addr = it->first.ip;

		char buf[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr_log, buf, INET_ADDRSTRLEN);

		Logger::info("Successfully opened socket for " + std::string(buf) + ":" + to_string(it->first.port));
	}
}

void Server::init()
{
	Logger::info("server starting");

	initInstances();
	initSockets();
}

void Server::run()
{
	running_ = true;

	Logger::info("server running, version " + std::string(VERSION));

	while (running_ && !siginted)
	{
		struct epoll_event events[MAX_EVENTS];
		int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);

		for (int i = 0; i < n; i++)
		{
			FdContext* context = static_cast<FdContext*>(events[i].data.ptr);
			if (!context) continue ;

			if (context->type == LISTEN)
			{
				int client_fd = accept(listen_fd_[context->fd_index], NULL, NULL);
				fcntl(client_fd, F_SETFL, O_NONBLOCK);

				connections_.insert(std::make_pair(client_fd, HttpConnection(client_fd, context, *this)));

				std::map<int, HttpConnection>::iterator it = connections_.find(client_fd);
				if (it == connections_.end())
				{
					Logger::error("can't add connection to the map. Aborting client connection.");
					close(client_fd);
					continue ;
				}
			
				if (addFdEpoll(client_fd, EPOLLIN | EPOLLOUT, it->second.getContext()) < 0)
				{
					Logger::error("epoll to client fd link failed. Aborting client connection.");
					close(client_fd);
					connections_.erase(it);
					continue ;
				}

				Logger::info("accepted new connection (fd: " + to_string(client_fd) + std::string(")"));
			}
			else if (context->type == CGI_IN || context->type == CGI_OUT)
			{
				handleCGI(events[i]);
			}
			else
			{
				handleClient(events[i]);
			}
		}
	}
}

Server::~Server()
{
	if (is_child_) return ;
	
	for (std::map<int, HttpConnection>::iterator it = connections_.begin(); it != connections_.end(); ++it)
	{
		removeFdEpoll(it->first);
	}
	clean();
}

void Server::clean()
{
	for (std::map<int, HttpConnection>::iterator it = connections_.begin(); it != connections_.end(); ++it)
	{
		if (it->first >= 0) close(it->first);
	}
	// if (listen_fd_ >= 0) close(listen_fd_);
	// listen_fd_ = -1; // TODO
	if (epoll_fd_ >= 0) close(epoll_fd_);
	epoll_fd_ = -1;
}
