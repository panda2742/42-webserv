
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
	// res_ready_ = false;
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
	std::cout << "vasy frere je prends la nouvelle requete" << std::endl;
	raw_.push_back('\0');
	requests_.push_back(HttpRequest());
	HttpRequest& req = requests_.back();

	req.init(raw_, header_size_, content_size_);
	req.parse();

	responses_.push_back(HttpResponse(req));
	HttpResponse& res = responses_.back();
	res.create();
	clear();
	// res_ready_ = true;
}

void HttpConnection::sendResponse()
{
	if (!responses_.empty())
	{
		HttpResponse& res = responses_.front();

		ResponseState state = res.sendResponsePart(socket_fd_);

		if (state == SENT)
		{
			responses_.pop_front();
			requests_.pop_front();
		}
		else if (state == ERROR)
		{
			responses_.pop_front();
			requests_.pop_front();
			// gerer la deco du client
		}
	}
	// clear();
}

