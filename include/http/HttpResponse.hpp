#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

#include <string>
#include <map>
#include <vector>
#include "http/HttpRequest.hpp"
#include "Logger.hpp"
#include "FileCacheManager.hpp"
#include "utils_structs.hpp"
#include <sys/stat.h>

#define CGI_TIMEOUT 2

/**
 * @enum ResponseState
 * @brief State of a response -> what I need to send.
 *
 * - NOT_SENT : Nothing have been sent
 * - HEADER : sending the header
 * - BODY : sending the body
 * - SENT : the response have been sent
 * - ERROR : an error occured while sending the request
 */
enum ResponseState
{
	NOT_SENT,
	HEADER,
	BODY,
	SENT,
	ERROR,
};

/**
 * @enum CGIState
 * @brief State of CGI handling for a response.
 *
 * - NO_CGI : no CGI associated
 * - SEND_BODY : send request body to the CGI process
 * - WAIT_CONTENT : waiting for content produced by the CGI
 * - CGI_FINISHED : CGI has finished producing output
 */
enum CGIState
{
	NO_CGI,
	SEND_BODY,
	WAIT_CONTENT,
	CGI_FINISHED
};

class Server;
struct FdContext;

/**
 * @class HttpResponse
 * @brief Create an HTTP response for a request
 *
 * This class have 2 majors parts :
 *
 * - The response creation : creating the headers, oppening the files asked by a get, ...
 *
 * - Sending the response. Each call to `sendResponsePart` will result in a send to the
 * client fd with a part of the response.
 */
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

	std::string root_;
	int depth_in_loc_;

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
	std::string path_info_;
	pid_t cgi_pid_;
	time_t cgi_create_time_;

	bool waiting_cgi_;
	bool res_ready_;

	/**
	 * Theses function have an enough explicit name
	 */
	void setHeader(const std::string &name, const std::string &value);
	void setStatus(int code, const std::string &message);
	void setStatus(int code);
	void setBody(const std::vector<char> &body);
	void handleExistingFile();
	void setDirectory();
	void setError(int code);
	void setRedirect(int code, const std::string& target);
	void addCookie(const std::string& name, const std::string& value, bool httpOnly, bool secure, ssize_t maxAge, const std::string& path, const std::string& sameSite);
	void useCGI(const std::string& cgi_prog, const std::string& script_path);
	void execChildCGI(const std::string& cgi_prog, const std::string& script_path);
	void handleResultCGI();

	/**
	 * calls one of the differents function above
	 */
	void createDefault();

	void serializeHeader();

	/**
	 * This send of data is a bit special because we never have the full file in memory.
	 * We read a small chunk of the file, send it, read another part, etc, so we never have
	 * more than 8ko of the file in memory
	 */
	bool sendFileDirectPart(int socket_fd);

	const std::string getBodySize() const;
	void getRealRoot();

	struct UploadExtractData {
		std::string			body;
		std::string			filename;
		size_t				body_size;
		unsigned short int	error;

		UploadExtractData(unsigned short int err): body(), filename(), body_size(0), error(err) {}
		UploadExtractData(const UploadExtractData& other): body(other.body), filename(other.filename), body_size(other.body_size), error(other.error) {}
		UploadExtractData& operator=(const UploadExtractData& other) {
			if (this != &other)
			{
				this->body = other.body;
				this->filename = other.filename;
				this->body_size = other.body_size;
			}
			return *this;
		}

		void	print(void) const
		{
			std::cout << "- Filename  = " << filename << "\n"
				<< "- Body size = " << body_size << " bytes\n"
				<< "- Error     = " << error << "\n"
				<< "- Body      = " << body << "\n";
		}
	};

public:
	HttpResponse(HttpRequest &req, Server &server);
	~HttpResponse();

	/**
	 * @brief Build the response from the associated request.
	 *	This method prepares headers and body according to the request and
	 *	the server configuration (static files, redirects, CGI, etc.). It does
	 *	not send any data on the socket itself.
	 */
	void create();

	/**
	 * @brief Send part of the response to the provided file descriptor.
	 * @param socket_fd Socket file descriptor to write to.
	 * @return the ResponseState after the operation.
	 *
	 * This function is intended to be called iteratively by the event loop:
	 * it may send the header first and then chunks of the body.
	 */
	ResponseState sendResponsePart(int socket_fd);

	void clear();

	/**
	 * @brief Send the request body to the CGI process (if used).
	 */
	void sendBodyCGI();

	/**
	 * @brief Read/collect the content produced by the CGI and integrate it into
	 * the response.
	 */
	void getContentCGI();

	void checkTimeoutCGI();

	std::deque<UploadExtractData>	extractUpload(char *body, size_t size) const;
};


#endif