
#include "HttpRequest.hpp"

HttpRequest::HttpRequest(std::vector<char>& raw, size_t header_size, size_t content_size)
	: raw_(raw), header_size_(header_size), content_size_(content_size)
{
	std::cout<<raw.data()<<std::endl;
	std::cout << method_ << ", " << header_size_ << ", " << content_size_ << std::endl;
	
}

void HttpRequest::parse()
{
	std::string header = std::string(raw_.begin(), raw_.begin() + header_size_);

	
}


/*
GET / HTTP/1.1
Host: localhost:8080
Content-Type: application/vnd.rar
User-Agent: insomnia/11.6.1
Accept: \*\/\*
Content-Length: 640065

*/

HttpRequest::~HttpRequest()
{
	
}

