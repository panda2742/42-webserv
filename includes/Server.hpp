#ifndef __SERVER_HPP__
# define __SERVER_HPP__

#include <vector>
#include <cstdlib>

class Server
{

private:

	bool running_;
	int listen_fd_;
	int epoll_fd_;
	
public:
	Server();
	~Server() {}

	void run();
};

#endif
