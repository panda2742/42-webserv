
#include "FileCacheManager.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <algorithm>

HttpRequest::HttpRequest()
	: create_error_(NO_REQ_ERROR)
{
	
}

void HttpRequest::init(std::vector<char>& raw, size_t header_size, size_t content_size)
{
	raw_ = raw;
	header_size_ = header_size;
	content_size_ = content_size;
}

const std::string* HttpRequest::getHeaderInfo(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = infos_.find(key);
	if (it == infos_.end()) return NULL;
	return &it->second;
}

bool isOnlyDigits(const std::string &s)
{
	for (size_t i = 0; i < s.size(); ++i)
        if (!isdigit((unsigned char)s[i]))
            return false;
    return true;
}

bool HttpRequest::checkHttpVersion()
{
	if (version_.empty())
	{
		create_error_ = NO_HTTP_VERSION;
		return false;
	}
	else
	{
		const std::string prefix = "HTTP/";
		if (version_.compare(0, prefix.size(), prefix) != 0)
		{
			create_error_ = BAD_REQUEST;
			return false;
		}

		std::string ver = version_.substr(prefix.size());

		size_t dot = ver.find('.');
		std::string major_str;
		int major = 0;
		int minor = 0;
		if (dot == std::string::npos)
		{
			major_str = ver;
			if (major_str.empty())
			{
				create_error_ = BAD_REQUEST;
				return false;
			}
		}
		else
		{
			major_str = ver.substr(0, dot);
			std::string minor_str = ver.substr(dot + 1);
	
			if (major_str.empty() || minor_str.empty())
			{
				create_error_ = BAD_REQUEST;
				return false;
			}
			
			if (!isOnlyDigits(minor_str))
			{
				create_error_ = BAD_REQUEST;
				return false;
			}
		
			minor = std::atoi(minor_str.c_str());
		}

		if (!isOnlyDigits(major_str))
		{
			create_error_ = BAD_REQUEST;
			return false;
		}
		major = std::atoi(major_str.c_str());

		if (major == 1 && minor == 1) return true;
	
		create_error_ = UNSUPPORTED_HTTP_VERSION;
		return false;
	}
}

std::string getNextPart(std::string& input, const std::string& sep)
{
	size_t next_space = input.find(sep);
	std::string res;

	if (next_space == std::string::npos)
	{
		res = input.substr(0, input.size());
		input.erase(0, input.size());
	}
	else
	{
		res = input.substr(0, next_space);
		input.erase(0, next_space + 1);
	}
	return res;
}

bool HttpRequest::parse()
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

		first_line_ = lines[0];
		
		std::string method = getNextPart(lines[0], " ");

		if (method == "GET") method_ = GET;
		else if (method == "POST") method_ = POST;
		else if (method == "DELETE") method_ = DELETE;
		else
		{
			method_ = UNKNOWN;
			create_error_ = UNKNOWN_METHOD;
			return true;
		}

		target_ = getNextPart(lines[0], " ");
		if (target_.empty())
		{
			create_error_ = NOT_HTTP_HEADER;
			return true;
		}
		else if (target_[0] != '/')
		{
			create_error_ = BAD_REQUEST;
			return true;
		}

		version_ = getNextPart(lines[0], " ");
		if (!checkHttpVersion()) return true;

		lines.erase(lines.begin());

		for (size_t i = 0; i < lines.size(); i++)
		{
			if (lines[i].find(": ") == std::string::npos)
			{
				create_error_ = BAD_REQUEST;
				break ;
			}
			std::string key = lines[i].substr(0, lines[i].find(": "));
			lines[i].erase(0, lines[i].find(": ") + 2);

			infos_.insert(std::make_pair(key, lines[i]));
		}

		std::map<std::string, std::string>::iterator it = infos_.find("Host");
		if (it == infos_.end())
		{
			create_error_ = BAD_REQUEST;
			return true;
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
		return (false);
	}
	return (true);
}

void HttpRequest::clear()
{
	raw_.clear();
	header_size_ = 0;
	content_size_ = 0;

	method_ = GET;
	target_.clear();
	version_.clear();

	infos_.clear();
}

HttpRequest::~HttpRequest()
{
	
}

