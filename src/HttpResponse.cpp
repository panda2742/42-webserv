#include "HttpResponse.hpp"
#include "FileCacheManager.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static const std::pair<const int, std::string> http_response_code_messages[] = {
	// 1xx: Informational
	std::pair<const int, std::string>(100, "Continue"),
	std::pair<const int, std::string>(101, "Switching Protocols"),
	std::pair<const int, std::string>(102, "Processing"),
	std::pair<const int, std::string>(103, "Early Hints"),

	// 2xx: Success
	std::pair<const int, std::string>(200, "OK"),
	std::pair<const int, std::string>(201, "Created"),
	std::pair<const int, std::string>(202, "Accepted"),
	std::pair<const int, std::string>(203, "Non-Authoritative Information"),
	std::pair<const int, std::string>(204, "No Content"),
	std::pair<const int, std::string>(205, "Reset Content"),
	std::pair<const int, std::string>(206, "Partial Content"),
	std::pair<const int, std::string>(207, "Multi-Status"),
	std::pair<const int, std::string>(208, "Already Reported"),
	std::pair<const int, std::string>(210, "Content Different"),
	std::pair<const int, std::string>(226, "IM Used"),

	// 3xx: Redirection
	std::pair<const int, std::string>(300, "Multiple Choices"),
	std::pair<const int, std::string>(301, "Moved Permanently"),
	std::pair<const int, std::string>(302, "Found"),
	std::pair<const int, std::string>(303, "See Other"),
	std::pair<const int, std::string>(304, "Not Modified"),
	std::pair<const int, std::string>(305, "Use Proxy"),
	std::pair<const int, std::string>(307, "Temporary Redirect"),
	std::pair<const int, std::string>(308, "Permanent Redirect"),
	std::pair<const int, std::string>(310, "Too many Redirects"),

	// 4xx: Client Error
	std::pair<const int, std::string>(400, "Bad Request"),
	std::pair<const int, std::string>(401, "Unauthorized"),
	std::pair<const int, std::string>(402, "Payment Required"),
	std::pair<const int, std::string>(403, "Forbidden"),
	std::pair<const int, std::string>(404, "Not Found"),
	std::pair<const int, std::string>(405, "Method Not Allowed"),
	std::pair<const int, std::string>(406, "Not Acceptable"),
	std::pair<const int, std::string>(407, "Proxy Authentication Required"),
	std::pair<const int, std::string>(408, "Request Timeout"),
	std::pair<const int, std::string>(409, "Conflict"),
	std::pair<const int, std::string>(410, "Gone"),
	std::pair<const int, std::string>(411, "Length Required"),
	std::pair<const int, std::string>(412, "Precondition Failed"),
	std::pair<const int, std::string>(413, "Payload Too Large"),
	std::pair<const int, std::string>(414, "URI Too Long"),
	std::pair<const int, std::string>(415, "Unsupported Media Type"),
	std::pair<const int, std::string>(416, "Range Not Satisfiable"),
	std::pair<const int, std::string>(417, "Expectation Failed"),
	std::pair<const int, std::string>(418, "I'm a teapot"),
	std::pair<const int, std::string>(421, "Misdirected Request"),
	std::pair<const int, std::string>(422, "Unprocessable Entity"),
	std::pair<const int, std::string>(423, "Locked"),
	std::pair<const int, std::string>(424, "Failed Dependency"),
	std::pair<const int, std::string>(425, "Too Early"),
	std::pair<const int, std::string>(426, "Upgrade Required"),
	std::pair<const int, std::string>(428, "Precondition Required"),
	std::pair<const int, std::string>(429, "Too Many Requests"),
	std::pair<const int, std::string>(431, "Request Header Fields Too Large"),
	std::pair<const int, std::string>(444, "No Response"),
	std::pair<const int, std::string>(451, "Unavailable For Legal Reasons"),
	std::pair<const int, std::string>(456, "Unrecoverable Error"),
	std::pair<const int, std::string>(495, "SSL Certificate Error"),
	std::pair<const int, std::string>(496, "SSL Certificate Required"),
	std::pair<const int, std::string>(497, "HTTP Request Sent to HTTPS Port"),
	std::pair<const int, std::string>(498, "Token expired/invalid"),
	std::pair<const int, std::string>(499, "Client Closed Request"),

	// 5xx: Server Error
	std::pair<const int, std::string>(500, "Internal Server Error"),
	std::pair<const int, std::string>(501, "Not Implemented"),
	std::pair<const int, std::string>(502, "Bad Gateway"),
	std::pair<const int, std::string>(503, "Service Unavailable"),
	std::pair<const int, std::string>(504, "Gateway Timeout"),
	std::pair<const int, std::string>(505, "HTTP Version Not Supported"),
	std::pair<const int, std::string>(506, "Variant Also Negotiates"),
	std::pair<const int, std::string>(507, "Insufficient Storage"),
	std::pair<const int, std::string>(508, "Loop Detected"),
	std::pair<const int, std::string>(510, "Not Extended"),
	std::pair<const int, std::string>(511, "Network Authentication Required")
};

const std::map<const int, std::string> HTTP_RESPONSE_MESSAGES(
	http_response_code_messages,
	http_response_code_messages + sizeof(http_response_code_messages) / sizeof(http_response_code_messages[0])
);

void HttpResponse::setStatus(int code, const std::string &message) {
	status_code_ = code;
	status_message_ = message;
}

void HttpResponse::setHeader(const std::string &name, const std::string &value) {
	headers_[name] = value;
}

void HttpResponse::setBody(const std::vector<char> &body) {
	body_ = body;
	headers_["Content-Length"] = to_string(body.size());
}

std::vector<char> HttpResponse::serialize() const
{
	std::vector<char> response;

	std::string status_line = "HTTP/1.1 " + to_string(status_code_) + " " + status_message_ + "\r\n";
	response.insert(response.end(), status_line.begin(), status_line.end());

	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		std::string header_line = it->first + ": " + it->second + "\r\n";
		response.insert(response.end(), header_line.begin(), header_line.end());
	}

	std::string crlf = "\r\n";
	response.insert(response.end(), crlf.begin(), crlf.end());
	response.insert(response.end(), body_.begin(), body_.end());

	return response;
}

void HttpResponse::setError(int code)
{
	std::map<const int, std::string>::const_iterator mime = HTTP_RESPONSE_MESSAGES.find(code);
	setStatus(code, mime == HTTP_RESPONSE_MESSAGES.end() ? "Unknown status code" : mime->second);
	
	// try load configurated error page
	// + set file status a FILE_OK si y'a bien un fichier / FILE_STREAM_DIRECT si trop lourd

	setHeader("Content-Type", "text/html");
	std::string error = to_string(code) + " " + status_message_;
	std::string body = "<html><head><title>"+error+"</title></head><body><center><h1>"+error+"</h1></center><hr><center>webserv/"+std::string(VERSION)+"</center></body></html>";
	std::vector<char> body_vec(body.begin(), body.end());
	setBody(body_vec);
}

void HttpResponse::serializeHeader()
{
	serialized_header_ = "HTTP/1.1 " + to_string(status_code_) + " " + status_message_ + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
		serialized_header_ += it->first + ": " + it->second + "\r\n";

	serialized_header_ += "\r\n";
}

void HttpResponse::create()
{
	if (req_.getMethod() == GET)
	{
		file_status_ = FileCacheManager::getFile(req_.getTarget(), file_, file_info_, file_path_);
		
		if (file_status_ == FILE_OK)
		{
			setStatus(200, "OK");
			setHeader("Content-Type", file_->mime);
			headers_["Content-Length"] = to_string(file_->size);
		}
		else if (file_status_ == FILE_STREAM_DIRECT)
		{
			direct_file_fd_ = open(file_path_.c_str(), O_RDONLY);
			
			if (direct_file_fd_ < 0)
				setError(500);
			else
			{
				setStatus(200, "OK");
				std::map<std::string, std::string>::const_iterator mime = MIME_TABLE.find(getExtension(req_.getTarget()));
				setHeader("Content-Type", mime == MIME_TABLE.end() ? "application/octet-stream" : mime->second);
				headers_["Content-Length"] = to_string(file_info_.st_size);
			}
		}
		else if (file_status_ == FILE_IS_DIR)
		{
			setStatus(200, "OK");
			setHeader("Content-Type", "text/plain");
			std::string body = "Rholala la grosse galere je gere pas encore les directory :/\nJe fais ca au plus vite mon bébou";
			std::vector<char> body_vec(body.begin(), body.end());
			setBody(body_vec);
		}
		else if (file_status_ == FILE_NOT_FOUND)
		{
			setError(404);
		}
		else if (file_status_ == FILE_FORBIDDEN || file_status_ == PATH_FORBIDDEN)
		{
			setError(404);
		}
		else if (file_status_ == PATH_TO_LONG)
		{
			setError(414);
		}
		else
		{
			setError(500);
		}

	}
	else
	{
		setHeader("Content-Type", "text/plain");
		std::string body = "Unknown method";
		std::vector<char> body_vec(body.begin(), body.end());
		setBody(body_vec);
	}

	serializeHeader();
	res_ready_ = true;
}

void HttpResponse::sendHeader(int socket_fd)
{
	std::string header;

	header = "HTTP/1.1 " + to_string(status_code_) + " " + status_message_ + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
		header += it->first + ": " + it->second + "\r\n";

	header += "\r\n";

	size_t total = 0;
	size_t to_send = header.size();

	while (total < to_send)
	{
		ssize_t sent = send(socket_fd, header.c_str() + total, to_send - total, 0);
		if (sent <= 0) break; // gerer erreur pour de vrai
		total += sent;
	}
}

void HttpResponse::sendBody(int socket_fd)
{
	size_t total = 0;
	size_t to_send = body_.size();

	while (total < to_send)
	{
		ssize_t sent = send(socket_fd, body_.data() + total, to_send - total, 0);
		if (sent <= 0) break; // gerer erreur pour de vrai
		total += sent;
	}
}

void HttpResponse::sendFileDirect(const std::string &path, int socket_fd)
{
	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0) return;

	char buffer[8192];
	ssize_t n;

	while ((n = read(fd, buffer, sizeof(buffer))) > 0)
	{
		ssize_t sent = 0;
		while (sent < n)
		{
			ssize_t s = send(socket_fd, buffer + sent, n - sent, 0);
			if (s <= 0)
			{
				close(fd); // aussi degager completement la connexion
				return ;
			}
			sent += s;
		}
	}

	close(fd);
}

void HttpResponse::sendResponse(int socket_fd)
{
	if (req_.getMethod() == GET)
	{
		if (file_status_ == FILE_OK)
		{
			sendHeader(socket_fd);

			size_t total = 0;
			size_t to_send = file_->data.size();

			while (total < to_send)
			{
				ssize_t sent = send(socket_fd, file_->data.data() + total, to_send - total, 0);
				if (sent <= 0) break;
				total += sent;
			}
		}
		else if (file_status_ == FILE_STREAM_DIRECT)
		{
			sendHeader(socket_fd);
			sendFileDirect(file_path_, socket_fd);
		}
		else
		{
			sendHeader(socket_fd);
			sendBody(socket_fd);
		}
	}
	else
	{
		
	}
}

bool HttpResponse::sendFileDirectPart(int socket_fd)
{
	if (send_index_ > 0)
	{
		if (send_index_ < (size_t)direct_file_n_)
		{
			ssize_t s = send(socket_fd, direct_file_buffer_ + send_index_, direct_file_n_ - send_index_, 0);
			if (s <= 0) return false;

			send_index_ += s;

			return true;
		}
		else
			send_index_ = 0;
	}
	
	direct_file_n_ = read(direct_file_fd_, direct_file_buffer_, sizeof(direct_file_buffer_));
	if (direct_file_n_ > 0)
	{
		ssize_t s = send(socket_fd, direct_file_buffer_ + send_index_, direct_file_n_ - send_index_, 0);
		if (s <= 0) return false;
		send_index_ += s;
	}
	else
		send_state_ = SENT;
	return true;
}

std::string respStateToText(ResponseState s)
{
	if (s == NOT_SENT) return "Not sent";
	if (s == HEADER) return "Header";
	if (s == BODY) return "Body";
	if (s == SENT) return "SENT";
	if (s == ERROR) return "err";
	return ("Bah wsh");
}

ResponseState HttpResponse::sendResponsePart(int socket_fd)
{
	if (!res_ready_) return ERROR;

	// std::cout << "Send state " << respStateToText(send_state_) << std::endl;

	if (send_state_ == NOT_SENT)
	{
		send_state_ = HEADER;
		send_index_ = 0;
	}

	if (send_state_ == HEADER)
	{
		size_t to_send = serialized_header_.size();

		if (send_index_ < to_send)
		{
			ssize_t sent = send(socket_fd, serialized_header_.c_str() + send_index_, to_send - send_index_, 0);

			if (sent <= 0) return ERROR; // gerer erreur pour de vrai

			send_index_ += sent;
		}
		else
		{
			send_state_ = BODY;
			send_index_ = 0;
		}
	}

	if (send_state_ == BODY)
	{
		if (req_.getMethod() == GET)
		{
			if (file_status_ == FILE_OK)
			{
				size_t to_send = file_->data.size();

				if (send_index_ < to_send)
				{
					ssize_t sent = send(socket_fd, file_->data.data() + send_index_, to_send - send_index_, 0);
					if (sent <= 0) return ERROR; // ON GERE CORRECTEMENT LES ERREURS STP
					send_index_ += sent;
				}
				else
				{
					send_state_ = SENT;
				}
			}
			else if (file_status_ == FILE_STREAM_DIRECT)
			{
				if (!sendFileDirectPart(socket_fd))
				{
					return ERROR;
					// gerer l'errreurrrr iciii aussi
				}
			}
			else
			{
				size_t to_send = body_.size();

				if (send_index_ < to_send)
				{
					ssize_t sent = send(socket_fd, body_.data() + send_index_, to_send - send_index_, 0);
					if (sent <= 0) return ERROR; // gerer erreur pour de vrai
					send_index_ += sent;
				}
				else
				{
					send_state_ = SENT;
				}
			}
		}
		else
		{
			
		}
	}

	return send_state_;
}

void HttpResponse::clear()
{
	status_code_ = 0;
	status_message_.clear();

	headers_.clear();
	body_.clear();

	if (direct_file_fd_ >= 0) close(direct_file_fd_);

	file_ = NULL;
	file_status_ = FILE_OK;
	file_path_.clear();

}
