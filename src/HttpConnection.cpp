
#include "HttpConnection.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <algorithm>
#include <vector>
#include <sys/socket.h>

HttpConnection::HttpConnection(int socket_fd) : socket_fd_(socket_fd), header_(false), content_size_(0)
{

}

HttpConnection::~HttpConnection()
{
	
}

void HttpConnection::clear()
{
	raw_.clear();
	header_ = false;
	content_size_ = 0;
}

ssize_t HttpConnection::find(const std::string& search, size_t range)
{
	std::vector<char>::iterator it;

	it = std::search(
		raw_.begin(), raw_.begin() + range,
		search.begin(), search.end()
	);

	if (it == raw_.begin() + range) return -1;

	return (std::distance(raw_.begin(), it));
}

std::string HttpConnection::findHeaderContent(const std::string& key, size_t range)
{
	ssize_t pos = find(key, range);

	if (pos == -1) return std::string();

	pos += key.length();

	while (pos < (ssize_t)range && raw_[pos] == ' ')
		pos++;

	const char crlf[] = "\r\n";

	std::vector<char>::iterator begin = raw_.begin() + pos;
	std::vector<char>::iterator end   = raw_.begin() + range;

	std::vector<char>::iterator it = std::search(
		begin, end,
		crlf, crlf + 2
	);
	
	return std::string(begin, it);
}

void HttpConnection::receiveContent(char *content, size_t size)
{
	raw_.insert(raw_.end(), content, content + size);

	if (!header_)
	{
		ssize_t pos = find("\r\n\r\n", raw_.size());

		if (pos == -1) return ;
		
		header_size_ = pos + 4;
		header_ = true;

		// Basic header parsing
		std::string contentLength = findHeaderContent("Content-Length:", pos);

		if (contentLength.empty())
		{
			handleRequest();
			return ;
		}

		content_size_ = std::atol(contentLength.c_str());
	}
	else
	{
		if (raw_.size() >= content_size_ + header_size_)
		{
			handleRequest();
			return ;
		}
	}
}

void HttpConnection::handleRequest()
{
	// std::cout << "Oeoe tkt je traite la requete" << std::endl;

	HttpRequest request(raw_, header_size_, content_size_);
	request.parse();

	HttpResponse response(request);
	response.create();
	response.sendResponse(socket_fd_);
	// std::vector<char> serialized = request.createResponse().serialize();

	// size_t total = 0;
	// size_t to_send = serialized.size();

	// while (total < to_send)
	// {
	// 	ssize_t sent = send(socket_fd_, serialized.data() + total, to_send - total, 0);
		
	// 	if (sent <= 0) break;

	// 	total += sent;
	// }

	// std::cout << findHeaderContent("User-Agent:", header_size_) << std::endl;

	clear();
}