#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

#include <string>
#include <map>
#include <vector>
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include "FileCacheManager.hpp"
#include <sys/stat.h>

enum ResponseState
{
	NOT_SENT,
	HEADER,
	BODY,
	SENT,
	ERROR,
};

class HttpResponse
{

private:
	HttpRequest& req_;

	int status_code_;
	std::string status_message_;

	std::map<std::string, std::string> headers_;
	std::vector<char> body_;

	CachedFile *file_;
	FileStatus file_status_;
	struct stat file_info_;
	std::string file_path_;

	std::string serialized_header_;

	ResponseState send_state_;
	size_t send_index_;
	int direct_file_fd_;
	ssize_t direct_file_n_;
	char direct_file_buffer_[8192];

	bool res_ready_;

	void setHeader(const std::string &name, const std::string &value);
	void setStatus(int code, const std::string &message);
	void setBody(const std::vector<char> &body);

	void createDefault();

	void setError(int code);
	
	void setDirectory();

	void serializeHeader();
	
	bool sendFileDirectPart(int socket_fd);

public:
	HttpResponse(HttpRequest &req)
		: req_(req), status_code_(500), file_(NULL), file_status_(NONE),
		send_state_(NOT_SENT), send_index_(0), direct_file_fd_(-1), direct_file_n_(-1), res_ready_(false) {}
	~HttpResponse() { clear(); }
	
	void create();
	ResponseState sendResponsePart(int socket_fd);

	void clear();

};


#endif