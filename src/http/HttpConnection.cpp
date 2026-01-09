#include "http/HttpConnection.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include <algorithm>
#include <vector>
#include <sys/socket.h>

HttpConnection::HttpConnection(int socket_fd, FdContext *socket_context, Server& server)
	: socket_fd_(socket_fd), socket_context_(socket_context), server_(server), header_(false), content_size_(0)
{
	context_.type = CLIENT;
	context_.fd = socket_fd;
}

HttpConnection::~HttpConnection()
{
	// requests_.clear();
	// std::deque<HttpRequest>().swap(requests_);
	// responses_.clear();
	// std::deque<HttpResponse>().swap(responses_);
	// raw_.clear();
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

bool HttpConnection::receiveContent(char *content, size_t size)
{
	if (!header_)
	{
		raw_.insert(raw_.end(), content, content + size);

		if (raw_.size() > 3)
		{
			if (raw_[0] == 0x16 && raw_[1] == 0x03 && (raw_[2] == 0x01 || raw_[2] == 0x02 || raw_[2] == 0x03))
			{
				handleRequest();
				return true;
			}
		}

		ssize_t pos = find("\r\n\r\n", raw_.size());

		if (pos == -1) return true;
		
		header_size_ = pos + 4;
		header_ = true;

		// Basic header parsing
		std::string contentLength = findHeaderContent("Content-Length:", pos);

		if (contentLength.empty()) content_size_ = 0;
		else content_size_ = std::atol(contentLength.c_str());

		if (!handleRequestHeader()) return false;
		
		if (content_size_ == 0)
		{
			handleRequest();
			return true;
		}
	}
	else
	{
		HttpRequest& req = requests_.back();

		if(req.getRealBodySize() + size > req.getLocation().getClientMaxBodySize())
		{
			req.setBodyTooLong();
			handleRequest();
			return true;
		}

		req.addBodyPart(content, size);

		if (req.isBodyFull())
		{
			handleRequest();
			return true;
		}
	}
	return true;
}

bool HttpConnection::handleRequestHeader()
{
	raw_.push_back('\0');
	requests_.push_back(HttpRequest());
	HttpRequest& req = requests_.back();

	req.init(raw_, header_size_, content_size_, socket_context_);

	if (!req.parse()) return false;

	return true;
}

void HttpConnection::handleRequest()
{
	HttpRequest& req = requests_.back();
	
	responses_.push_back(HttpResponse(req, server_));
	HttpResponse& res = responses_.back();
	res.create();

	clear();
}

bool HttpConnection::sendResponse()
{
	if (!responses_.empty())
	{
		HttpResponse& res = responses_.front();

		ResponseState state = res.sendResponsePart(socket_fd_);

		if (state == SENT)
		{
			HttpRequest& req = requests_.front();
			const std::string* keepalive = req.getHeaderInfo("Connection");
			if (keepalive && *keepalive == "close") return false;

			responses_.pop_front();
			requests_.pop_front();
		}
		else if (state == ERROR)
		{
			responses_.pop_front();
			requests_.pop_front();
			return false;
		}
	}
	return true;
}

