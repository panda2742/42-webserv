#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

#include <string>
#include <map>
#include <vector>

class HttpResponse
{

private:
	int status_code_;
	std::string status_message_;

	std::map<std::string, std::string> headers_;
	std::vector<char> body_;

public:
	HttpResponse(int code, const std::string &message)
		: status_code_(code), status_message_(message) {}
	~HttpResponse() {}

	void setHeader(const std::string &name, const std::string &value);
	void setBody(const std::vector<char> &body);

	std::vector<char> serialize() const;

};


#endif