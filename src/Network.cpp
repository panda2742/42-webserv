
#include <Network.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <stdexcept>
#include <string>

#define BASEPORT 8080

Network::Network()
{
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Socket options pour ne pas bloquer le port apres un kill
	int opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(BASEPORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error("Socket binding: " + std::string(strerror(errno)));
	}

	if (listen(listen_fd, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");


}
