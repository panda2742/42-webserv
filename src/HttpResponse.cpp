#include "HttpResponse.hpp"
#include "utils.hpp"

void HttpResponse::setHeader(const std::string &name, const std::string &value) {
	headers_[name] = value;
}

void HttpResponse::setBody(const std::vector<char> &body) {
	body_ = body;
	headers_["Content-Length"] = to_string(body.size());
}

std::vector<char> HttpResponse::serialize() const
{
	std::vector<char> response;

	std::string status_line = "HTTP/1.1 " + to_string(status_code_) + " " + status_message_ + "\r\n";
	response.insert(response.end(), status_line.begin(), status_line.end());

	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		std::string header_line = it->first + ": " + it->second + "\r\n";
		response.insert(response.end(), header_line.begin(), header_line.end());
	}

	response.insert(response.end(), "\r\n", "\r\n" + 2);
	response.insert(response.end(), body_.begin(), body_.end());

	return response;
}
