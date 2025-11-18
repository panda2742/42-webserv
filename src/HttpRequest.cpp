
#include "FileCacheManager.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <algorithm>

HttpRequest::HttpRequest(std::vector<char>& raw, size_t header_size, size_t content_size)
	: raw_(raw), header_size_(header_size), content_size_(content_size)
{

}

void HttpRequest::parse()
{
	try
	{
		std::string header = std::string(raw_.begin(), raw_.begin() + header_size_ - 2);

		std::vector<std::string> lines;

		size_t pos = 0;
		while ((pos = header.find("\r\n")) != std::string::npos)
		{
			lines.push_back(header.substr(0, pos));
			header.erase(0, pos + 2);
		}

		Logger::info(lines[0]);

		std::string method = lines[0].substr(0, lines[0].find(" ")); lines[0].erase(0, lines[0].find(" ") + 1);
		

		if (method == "GET") method_ = GET;
		else if (method == "POST") method_ = POST;
		else if (method == "DELETE") method_ = DELETE;

		target_ = lines[0].substr(0, lines[0].find(" ")); lines[0].erase(0, lines[0].find(" ") + 1);
		version_ = lines[0];

		lines.erase(lines.begin());

		for (size_t i = 0; i < lines.size(); i++)
		{
			std::string key = lines[i].substr(0, lines[i].find(": "));
			lines[i].erase(0, lines[i].find(": ") + 2);

			infos_.insert(std::make_pair(key, lines[i]));
		}

		// for (std::map<std::string, std::string>::const_iterator it = infos_.begin();
		// 	it != infos_.end();
		// 	++it)
		// {
		// 	std::cout << it->first << ": " << it->second << std::endl;
		// }

	}
	catch(const std::exception& e)
	{
		Logger::error("Wrong HTTP request format. " + std::string(e.what()));
	}
}

HttpRequest::~HttpRequest()
{
	
}

