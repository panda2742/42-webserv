
#include "FileCacheManager.hpp"
#include "http/HttpRequest.hpp"
#include "utils.hpp"
#include "Logger.hpp"
#include <algorithm>

HttpRequest::HttpRequest()
	: create_error_(NO_REQ_ERROR), location(NULL)
{

}

void HttpRequest::init(std::vector<char>& raw, size_t header_size, size_t content_size, FdContext *socket_context, FdContext *connection_context)
{
	raw_ = raw;
	header_size_ = header_size;
	content_size_ = content_size;
	socket_context_ = socket_context;
	connection_context_ = connection_context;
}

const std::string* HttpRequest::getHeaderInfo(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = infos_.find(key);
	if (it == infos_.end()) return NULL;
	return &it->second;
}

const std::string* HttpRequest::getCookie(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = cookies_.find(key);
	if (it == cookies_.end()) return NULL;
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

bool HttpRequest::parseTarget()
{
	if (target_.empty())
	{
		create_error_ = NOT_HTTP_HEADER;
		return false;
	}
	else if (target_[0] != '/')
	{
		create_error_ = BAD_REQUEST;
		return false;
	}

	size_t queries_start = target_.find("?");
	if (queries_start == std::string::npos) return true;

	std::string raw_queries = target_.substr(queries_start + 1);
	target_ = target_.substr(0, queries_start);

	while (!raw_queries.empty())
	{
		std::string query = getNextPart(raw_queries, "&");

		size_t equal = query.find("=");

		if (equal == std::string::npos) queries_[query] = "";
		else queries_[query.substr(0, equal)] = query.substr(equal + 1);
	}
	return true;
}

bool HttpRequest::linkInstance()
{
	std::map<std::string, std::string>::iterator host_it = infos_.find("Host");
	if (host_it == infos_.end())
	{
		create_error_ = BAD_REQUEST;
		return true;
	}

	size_t pos = host_it->second.find(":");
	std::string req_host;
	if (pos == std::string::npos) req_host = host_it->second;
	else req_host = host_it->second.substr(0, pos);

	const std::vector<ServerInstance*>& instances = *socket_context_->server_instances;
	ServerInstance *first_default = NULL;
	instance_ = NULL;

	for (
		std::vector<ServerInstance*>::const_iterator it = instances.begin();
		it != instances.end();
		++it
	) {
		const std::vector<std::string>& serv_names = (*it)->getServerNames();

		if (!first_default && (*it)->hasDefaultName()) first_default = *it;

		for (
			std::vector<std::string>::const_iterator host_name = serv_names.begin();
			host_name != serv_names.end();
			++host_name
		) {
			if (*host_name == req_host)
			{
				instance_ = *it;
				break ;
			}
		}

		if (instance_) break ;
	}
	if (!instance_) instance_ = first_default ? first_default : instances[0];

	// std::cout << cfg::util::represent(instance_->getRoot()) << std::endl;

	// std::cout << "Host: " << host_it->second << std::endl;
	return false;
}

bool HttpRequest::parse()
{
	location = ServerInstance::getGlobalLocation();
	try
	{
		if (raw_.size() > 3 && raw_[0] == 0x16 && raw_[1] == 0x03 && (raw_[2] == 0x01 || raw_[2] == 0x02 || raw_[2] == 0x03))
		{
			create_error_ = HTTPS_REQUEST;
			return true;
		}

		std::string header = std::string(raw_.begin(), raw_.begin() + header_size_ - 2);

		std::vector<std::string> lines;

		size_t pos = 0;
		while ((pos = header.find("\r\n")) != std::string::npos)
		{
			lines.push_back(header.substr(0, pos));
			header.erase(0, pos + 2);
		}

		first_line_ = lines[0];

		if (split(first_line_, ' ').size() < 2)
		{
			create_error_ = BAD_REQUEST;
			return true;
		}

		std::string method = getNextPart(lines[0], " ");

		if (method == "GET") method_ = METHOD_GET;
		else if (method == "POST") method_ = METHOD_POST;
		else if (method == "DELETE") method_ = METHOD_DELETE;
		else
		{
			method_ = 0;
			create_error_ = UNKNOWN_METHOD;
			return true;
		}

		target_ = getNextPart(lines[0], " ");
		if (!parseTarget()) return true;

		version_ = getNextPart(lines[0], " ");

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

		if (linkInstance()) return true;

		std::vector<std::string> splitted_target = split(target_, '/');
		location = &instance_->getLocations().matches(splitted_target);

		if (!checkHttpVersion()) return true;

		// std::cout << "==== START TEST MATCH ====" << std::endl;
		// location->print();
		// std::cout << "==== END TEST MATCH ====" << std::endl;

		const std::string *cookies = getHeaderInfo("Cookie");
		if (cookies)
		{
			std::vector<std::string> cookies_split = split(*cookies, ';');

			for (std::vector<std::string>::iterator it = cookies_split.begin(); it != cookies_split.end(); it++)
			{
				*it = trim(*it);
				size_t eq_pos = it->find('=');
				if (eq_pos == std::string::npos) continue;

				cookies_[it->substr(0, eq_pos)] = it->substr(eq_pos + 1);
			}
		}

		// for (std::map<std::string, std::string>::const_iterator it = infos_.begin();
		// 	it != infos_.end();
		// 	++it)
		// {
		// 	std::cout << it->first << ": " << it->second << std::endl;
		// }

		// for (std::map<std::string, std::string>::const_iterator itt = queries_.begin();
		// 	itt != queries_.end();
		// 	++itt)
		// {
		// 	std::cout << itt->first << ": " << itt->second << std::endl;
		// }

		// for (std::map<std::string, std::string>::const_iterator it = cookies_.begin();
		// 	it != cookies_.end();
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

	method_ = METHOD_GET;
	target_.clear();
	version_.clear();

	infos_.clear();
}

HttpRequest::~HttpRequest()
{

}

