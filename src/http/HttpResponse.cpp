#include "http/HttpResponse.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

HttpResponse::HttpResponse(HttpRequest &req, Server& server)
		: server_(server),
		req_(req),
		status_code_(500),
		status_mutable_(true),
		file_(NULL),
		file_status_(NONE),
		send_state_(NOT_SENT),
		cgi_state_(NO_CGI),
		send_index_(0),
		direct_file_fd_(-1),
		direct_file_n_(-1),
		cgi_in_(-1),
		cgi_out_(-1),
		waiting_cgi_(false),
		res_ready_(false)
{}

HttpResponse::~HttpResponse()
{
	clear();
}

void HttpResponse::serializeHeader()
{
	serialized_header_ = "HTTP/1.1 " + to_string(status_code_) + " " + status_message_ + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
		serialized_header_ += it->first + ": " + it->second + "\r\n";

	for (std::vector<ResCookie>::const_iterator it = cookies_.begin(); it != cookies_.end(); ++it)
	{
		std::string cookie = "Set-Cookie: "+ it->name +"="+ it->value;

		if (it->httpOnly) cookie += "; HttpOnly";
		if (it->secure) cookie += "; Secure";
		if (it->maxAge > 0) cookie += "; Max-Age=" + to_string(it->maxAge);
		if (!it->path.empty()) cookie += "; Path=" + it->path;
		if (!it->sameSite.empty()) cookie += "; SameSite=" + it->sameSite;

		serialized_header_ += cookie + "\r\n";
	}

	serialized_header_ += "\r\n";
}

const std::string HttpResponse::getBodySize() const
{
	std::map<std::string, std::string>::const_iterator it = headers_.find("Content-Length");
	if (it == headers_.end()) return "0";
	return it->second;
}

void HttpResponse::clear()
{
	status_code_ = 0;
	status_message_.clear();

	headers_.clear();
	body_.clear();

	if (direct_file_fd_ >= 0) {
		close(direct_file_fd_);
		direct_file_fd_ = -1;
	}

	if (cgi_in_ >= 0)
	{
		server_.removeCgiFd(cgi_in_);
		close(cgi_in_);
		cgi_in_ = -1;
	}
	if (cgi_out_ >= 0)
	{
		server_.removeCgiFd(cgi_out_);
		close(cgi_out_);
		cgi_out_ = -1;
	}

	file_ = NULL;
	file_status_ = FILE_OK;
	file_path_.clear();

}
