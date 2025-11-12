#ifndef __NETWORK_HPP__
# define __NETWORK_HPP__

#include <vector>
#include <cstdlib>

class Network
{

private:
	std::vector<int> fds;
	size_t nfds;
	
public:
	Network();
	~Network() {}

};

#endif
