#ifndef __HTTP_CONNECTION_HPP__
# define __HTTP_CONNECTION_HPP__

#include <vector>
#include <iostream>
#include <deque>
#include <sys/types.h>
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "utils_structs.hpp"

class Server;

/**
 * @class HttpConnection
 * @brief Represents a TCP connection between a client and the HTTP server.
 *
 * HttpConnection manages the state of a socket connection (incoming and
 * outgoing data), assembles raw received bytes into HTTP requests
 * (`HttpRequest`) and holds a queue of current requests and prepared responses
 * (`HttpResponse`).
 *
 * The object maintains a raw buffer (`raw_`) used to parse headers and body,
 * and two FIFO queues (`requests_`, `responses_`) to decouple network I/O
 * from request processing. Actual read/write operations are performed by the
 * surrounding event loop in the server.
 */
class HttpConnection
{
private:
	int socket_fd_;
	FdContext context_;
	FdContext *socket_context_;
	
	Server& server_;

	std::vector<char> raw_;
	bool header_;
	size_t content_size_;
	size_t header_size_;

	ssize_t find(const std::string& search, size_t range);
	std::string findHeaderContent(const std::string& key, size_t range);

	std::deque<HttpRequest> requests_;
	std::deque<HttpResponse> responses_;

	/**
	 * Called when the request is considered as completed.
	 * It create a `HttpRequest`, parse it, and finnaly create a `HttpResponse`.
	 */
	void handleRequest();
	bool handleRequestHeader();
	
public:
	HttpConnection(int socket_fd, FdContext *socket_context, Server& server);
	~HttpConnection();

	/**
	 * Get the fd context of the connection. Each fd in epoll MUST have an assiciated `FdContext`.
	 * It is stored in the epoll data.ptr, and contain information on the fd, such as the type of
	 * fd : client, cgi pipe, etc.
	 */
	FdContext* getContext() { return &context_; }

	/**
	 * Get the fd context of the socket of the connection. It's type is always LISTEN and contain the
	 * instances informations
	 */
	FdContext* getSocketContext() { return socket_context_; }

	/**
	 * This function is called when epoll mark the fd of the connection as ready to read.
	 * This function just take as a parameter the result of recv.
	 * 
	 * @param content The data received by recv
	 * @param size The size of the received data
	 */
	bool receiveContent(char *content, size_t size);

	/**
	 * This function is called when epoll mark the fd of the connection as ready to write.
	 * If the connection have pending response in the response queue, it send a part of the
	 * response it need to send.
	 */
	bool sendResponse();
	
	void clear();

};

#endif
