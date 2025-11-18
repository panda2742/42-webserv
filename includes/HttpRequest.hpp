#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

#include <vector>
#include <iostream>
#include <sys/types.h>
#include <map>

enum Method {
	GET,
	POST,
	DELETE
};

class HttpRequest
{

private:

	std::vector<char> raw_;
	size_t header_size_;
	size_t content_size_;

	Method method_;
	std::string target_;
	std::string version_;

	std::map<std::string, std::string>infos_;

public:
	HttpRequest();
	~HttpRequest();

	void init(std::vector<char>& raw, size_t header_size, size_t content_size);
	void parse();

	std::string getTarget() { return target_; };
	Method getMethod() { return method_; };
	size_t getContentSize() { return content_size_; }

	void clear();

};

#endif
