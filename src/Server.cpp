
#include <Server.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <stdexcept>
#include <string>
#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>

#define BASEPORT 8080
#define MAX_EVENTS 64

void handle_client(int client_fd)
{
	char buffer[4096];
	ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0) {
		close(client_fd);
		return;
	}

	buffer[bytes] = '\0';
	std::cout << "=== Requête reçue ===" << std::endl;
	std::cout << buffer << std::endl;
}


Server::Server()
	: running_(false)
{
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

	epoll_fd_ = epoll_create(0);
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

	while (running_)
	{
		struct epoll_event events[MAX_EVENTS];
		int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
		for (int i = 0; i < n; i++) {
			if (events[i].data.fd == listen_fd_) {
				// Nouvelle connexion entrante
				int client_fd = accept(listen_fd_, nullptr, nullptr);
				fcntl(client_fd, F_SETFL, O_NONBLOCK);

				struct epoll_event client_ev;
				client_ev.events = EPOLLIN | EPOLLET; // lecture en mode edge-triggered
				client_ev.data.fd = client_fd;
				epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &client_ev);
			} else {
				// Données à lire sur un client existant
				handle_client(events[i].data.fd);
			}
		}
	}
}
