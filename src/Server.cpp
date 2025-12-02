
#include "Server.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <stdexcept>
#include <string>
#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define BASEPORT 8080
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

	while ((r = recv(fd, buf, sizeof(buf), 0)) > 0)
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

Server::Server()
	: running_(false), is_child_(false)
{
	listen_fd_ = -1;
	epoll_fd_ = -1;
}

void Server::init()
{
	Logger::info("server starting");

	listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);

	// Socket options pour ne pas bloquer le port apres un kill
	int opt = 1;
	setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(BASEPORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Socket binding: " + std::string(strerror(errno)));

	if (listen(listen_fd_, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");

	fcntl(listen_fd_, F_SETFL, O_NONBLOCK);

	epoll_fd_ = epoll_create(1);
	if (epoll_fd_ < 0)
		throw std::runtime_error("epoll_create failed");

	listen_context_.type = LISTEN;
	listen_context_.fd = listen_fd_;

	if (addFdEpoll(listen_fd_, EPOLLIN, &listen_context_) < 0)
		throw std::runtime_error("epoll to listen fd link failed");
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
				int client_fd = accept(listen_fd_, NULL, NULL);
				fcntl(client_fd, F_SETFL, O_NONBLOCK);

				connections_.insert(std::make_pair(client_fd, HttpConnection(client_fd, *this)));

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
	if (listen_fd_ >= 0) close(listen_fd_);
	listen_fd_ = -1;
	if (epoll_fd_ >= 0) close(epoll_fd_);
	epoll_fd_ = -1;
}
