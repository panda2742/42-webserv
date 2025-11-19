
#include "Server.hpp"
#include "Logger.hpp"
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

#define BASEPORT 8080
#define MAX_EVENTS 64

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

	if (r == 0)
	{
 		close(fd);
 		std::map<int, HttpConnection>::iterator it = connections_.find(fd);
 		if (it != connections_.end()) {
 			it->second.clear();
 			connections_.erase(it);
 		}
		return;
	}

	std::map<int, HttpConnection>::iterator it = connections_.find(fd);
	if (it != connections_.end()) {
		it->second.receiveContent(request_buffer.data(), size);
	} else {
		Logger::warn("received data for unknown fd");
	}
}

void Server::handleClientOUT(int fd)
{
	std::map<int, HttpConnection>::iterator it = connections_.find(fd);
	if (it == connections_.end()) {
		Logger::warn("received data for unknown fd");
		return ;
	}

	it->second.sendResponse();
}

void Server::handleClient(struct epoll_event& epoll)
{
	if (epoll.events & (EPOLLERR | EPOLLHUP))
	{
		// closeConnection(epoll.data.fd); gerer une deco
		return;
	}

	if (epoll.events & EPOLLIN) {
		handleClientIN(epoll.data.fd);   // lecture + parsing + queue des requêtes
	}

	if (epoll.events & EPOLLOUT) {
		handleClientOUT(epoll.data.fd);  // machine à états d'envoi
	}
}



Server::Server()
	: running_(false)
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

	if (bind(listen_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error("Socket binding: " + std::string(strerror(errno)));
	}

	if (listen(listen_fd_, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");

	fcntl(listen_fd_, F_SETFL, O_NONBLOCK);

	epoll_fd_ = epoll_create(1);
	if (epoll_fd_ < 0)
		throw std::runtime_error("epoll_create failed");

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = listen_fd_;
	epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, listen_fd_, &ev);

}

void Server::run()
{
	running_ = true;

	Logger::info("server running, version " + std::string(VERSION));

	while (running_)
	{
		struct epoll_event events[MAX_EVENTS];
		int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);

		for (int i = 0; i < n; i++)
		{
			if (events[i].data.fd == listen_fd_)
			{
				int client_fd = accept(listen_fd_, NULL, NULL);
				fcntl(client_fd, F_SETFL, O_NONBLOCK);

				struct epoll_event client_ev;
				client_ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
				client_ev.data.fd = client_fd;
				epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &client_ev);

				Logger::info("accepted new connection");
				connections_.insert(std::make_pair(client_fd, HttpConnection(client_fd)));
			}
			else
			{
				handleClient(events[i]);
			}
		}
	}
}
