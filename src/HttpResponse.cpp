#include "HttpResponse.hpp"
#include "FileCacheManager.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

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

void HttpResponse::setError(int code)
{
	setStatus(code, getHttpErrorMessage(code));
	
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
				setHeader("Content-Type", getMimeType(getExtension(req_.getTarget())));
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

	//std::cout << "Send state " << respStateToText(send_state_) << std::endl;

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
