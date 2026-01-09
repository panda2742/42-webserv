#include "http/HttpResponse.hpp"
#include "FileCacheManager.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <unistd.h>

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

ResponseState HttpResponse::sendResponsePart(int socket_fd)
{
	if (waiting_cgi_) return NOT_SENT;
	if (!res_ready_) return ERROR;

	// std::cout << "Send state " << respStateToText(send_state_) << std::endl;

	if (send_state_ == NOT_SENT)
	{
		Logger::info("\"" + req_.getFirstLine() + "\" " + to_string(status_code_) + " " + getBodySize());

		send_state_ = HEADER;
		if (req_.getRequestError() == NO_HTTP_VERSION) send_state_ = BODY;

		send_index_ = 0;
	}

	if (send_state_ == HEADER)
	{
		size_t to_send = serialized_header_.size();

		if (send_index_ < to_send)
		{
			ssize_t sent = send(socket_fd, serialized_header_.c_str() + send_index_, to_send - send_index_, 0);

			if (sent <= 0) return ERROR;

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
		if (req_.getMethod() == METHOD_GET)
		{
			if (file_status_ == FILE_OK)
			{
				size_t to_send = file_->data.size();

				if (send_index_ < to_send)
				{
					ssize_t sent = send(socket_fd, file_->data.data() + send_index_, to_send - send_index_, 0);
					if (sent <= 0) return ERROR;
					send_index_ += sent;
				}
				else send_state_ = SENT;
			}
			else if (file_status_ == FILE_STREAM_DIRECT)
			{
				if (!sendFileDirectPart(socket_fd)) return ERROR;
			}
			else
			{
				size_t to_send = body_.size();

				if (send_index_ < to_send)
				{
					ssize_t sent = send(socket_fd, body_.data() + send_index_, to_send - send_index_, 0);
					if (sent <= 0) return ERROR;
					send_index_ += sent;
				}
				else send_state_ = SENT;
			}
		}
		else
		{

		}
	}

	return send_state_;
}
