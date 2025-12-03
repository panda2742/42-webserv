#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

#include <string>
#include <map>
#include <vector>
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include "FileCacheManager.hpp"
#include "utils_structs.hpp"
#include <sys/stat.h>

enum ResponseState
{
	NOT_SENT,
	HEADER,
	BODY,
	SENT,
	ERROR,
};

enum CGIState
{
	NO_CGI,
	SEND_BODY,
	WAIT_CONTENT,
	CGI_FINISHED
};

class Server;
struct FdContext;

class HttpResponse
{

private:
	struct ResCookie
	{
		std::string name;
		std::string value;
		bool httpOnly;
		bool secure;
		ssize_t maxAge;
		std::string path;
		std::string sameSite;
	};

	Server& server_;
	HttpRequest& req_;

	int status_code_;
	std::string status_message_;

	std::map<std::string, std::string> headers_;
	std::vector<ResCookie> cookies_;
	std::vector<char> body_;

	CachedFile *file_;
	FileStatus file_status_;
	struct stat file_info_;
	std::string file_path_;

	std::string serialized_header_;

	ResponseState send_state_;
	CGIState cgi_state_;
	size_t send_index_;
	int direct_file_fd_;
	ssize_t direct_file_n_;
	char direct_file_buffer_[8192];

	int cgi_in_;
	int cgi_out_;
	FdContext fd_context_in_;
	FdContext fd_context_out_;

	bool waiting_cgi_;
	bool res_ready_;

	void setHeader(const std::string &name, const std::string &value);
	void setStatus(int code, const std::string &message);
	void setBody(const std::vector<char> &body);
	void setDirectory();
	void setError(int code);
	void setRedirect(int code, const std::string& target);
	void addCookie(const std::string& name, const std::string& value, bool httpOnly, bool secure, ssize_t maxAge, const std::string& path, const std::string& sameSite);
	void useCGI(const std::string& cgi_prog, const std::string& script_path);
	void execChildCGI(const std::string& cgi_prog, const std::string& script_path);
	void handleResultCGI();

	void createDefault();

	void serializeHeader();
	bool sendFileDirectPart(int socket_fd);

	const std::string getBodySize() const;

public:
	HttpResponse(HttpRequest &req, Server &server);
	~HttpResponse();
	
	void create();
	ResponseState sendResponsePart(int socket_fd);

	void clear();

	void sendBodyCGI();
	void getContentCGI();

};


#endif