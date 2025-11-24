#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

#include <vector>
#include <iostream>
#include <sys/types.h>
#include <map>

enum Method {
	UNKNOWN,
	GET,
	POST,
	DELETE
};

enum RequestError
{
	NO_REQ_ERROR,
	NOT_HTTP_HEADER,
	UNKNOWN_METHOD,
	UNSUPPORTED_HTTP_VERSION,
	NO_HTTP_VERSION,
	BAD_REQUEST
};

class HttpRequest
{

private:

	std::vector<char> raw_;
	size_t header_size_;
	size_t content_size_;

	std::string first_line_;
	Method method_;
	std::string target_;
	std::string version_;

	RequestError create_error_;

	std::map<std::string, std::string>infos_;

	bool checkHttpVersion();

public:
	HttpRequest();
	~HttpRequest();

	void init(std::vector<char>& raw, size_t header_size, size_t content_size);
	bool parse();

	std::string getTarget() { return target_; }
	Method getMethod() { return method_; }
	size_t getContentSize() { return content_size_; }
	RequestError getRequestError() { return create_error_; }
	std::string& getFirstLine() { return first_line_; }
	const std::string* getHeaderInfo(const std::string& key) const;

	void clear();

};

#endif
