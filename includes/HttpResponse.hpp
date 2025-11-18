#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

#include <string>
#include <map>
#include <vector>
#include "HttpRequest.hpp"
#include "FileCacheManager.hpp"
#include <sys/stat.h>

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

	void setHeader(const std::string &name, const std::string &value);
	void setStatus(int code, const std::string &message);
	void setBody(const std::vector<char> &body);

	void setError(int code);

	void sendHeader(int socket_fd);
	void sendBody(int socket_fd);

public:
	HttpResponse(HttpRequest &req)
		: req_(req), status_code_(500) {}
	~HttpResponse() {}

	std::vector<char> serialize() const;
	
	void create();
	void sendResponse(int socket_fd);

};


#endif