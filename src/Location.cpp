#include "Location.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <cstring>

Location::Location(StrDirective& directive, Location *location) : directive_(directive), parent_(location), type_(LOCATION_DEFAULT)
{
}

void Location::init()
{
	// --------------- ROUTE --------------- //
	if (parent_ != NULL &&  parent_ != ServerInstance::getGlobalLocation())
		route_ = split(directive_.value, '/');

	// ---------------- ROOT --------------- //
	try
	{
		if (parent_)
		{
			std::vector<std::string> root_vec = directive_.get<std::vector<std::string> >("root");
			if (root_vec.size() < 1)
			{
				root_ = parent_->root_;
				for (std::vector<std::string>::const_iterator rit(route_.begin()); rit != route_.end(); rit++)
					root_ += '/' + *rit;
			}
			else 
				root_ = root_vec.at(0);
	
			if (root_.empty()) throw std::invalid_argument("root is required");
	
			if (root_.size() - 1 == '/') root_.erase(root_.end() - 1);
		}
	}
	catch (const std::exception& e) {
		throw std::invalid_argument("Invalid root value in location. Error: " + std::string(e.what()));
	}

	// ------------ ERROR PAGES ------------ //
	try {
		error_pages_ = directive_.get<std::map<unsigned int, std::string> >("error_page");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid error_page value in location. Error: " + std::string(e.what()));
	}


	// -------- CLIENT MAX BODY SIZE ------- //
	std::vector<std::string> client_max_body_size;
	try {
		client_max_body_size = directive_.get<std::vector<std::string> >("client_max_body_size");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid client_max_body_size value in location. Error: " + std::string(e.what()));
	}

	if (client_max_body_size.size() < 1)
	{
		if (!parent_) client_max_body_size_ = DEFAULT_CLIENT_MAX_BODY_SIZE;
		else client_max_body_size_ = parent_->client_max_body_size_;
	}
	else
	{
		std::string& el = client_max_body_size.at(0);

		unsigned long multiply = 1;
		for (size_t i = 0; i < el.size(); i++)
		{
			if (!isdigit(el[i]))
			{
				if (i == 0) throw std::invalid_argument("Invalid client_max_body_size value: " + el);
				else if (multiply != 1) throw std::invalid_argument("Invalid client_max_body_size value: " + el);
				else if (el[i] == 'K') multiply = 1024;
				else if (el[i] == 'M') multiply = 1024 * 1024;
				else if (el[i] == 'G') multiply = 1024 * 1024 * 1024;
				else throw std::invalid_argument("Invalid client_max_body_size value: " + el);
			}
		}
		client_max_body_size_ = std::atol(el.c_str()) * multiply;
	}


	// ----------- ALLOW METHODS ----------- //
	std::vector<std::string> allow_methods;
	try {
		allow_methods = directive_.get<std::vector<std::string> >("allow_methods");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid allow_methods value in location. Error: " + std::string(e.what()));
	}

	if (allow_methods.size() < 1)
	{
		if (!parent_) allow_methods_ = 255;
		else allow_methods_ = parent_->allow_methods_;
	}
	else
	{
		allow_methods_ = 0;

		for (std::vector<std::string>::iterator it = allow_methods.begin(); it != allow_methods.end(); it++)
		{
			if (*it == "GET") allow_methods_ |= METHOD_GET;
			else if (*it == "POST") allow_methods_ |= METHOD_POST;
			else if (*it == "DELETE") allow_methods_ |= METHOD_DELETE;
			else throw std::invalid_argument("Unknown method: " + *it);
		}
	}

	// -------------- REDIRECT ------------- //
	std::vector<std::string> redirect;
	try {
		redirect = directive_.get<std::vector<std::string> >("return");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid return value in location. Error: " + std::string(e.what()));
	}

	redirection_.enabled = false;

	if (redirect.size() != 0 && redirect.size() != 2) throw std::invalid_argument("Invalid return value");
	else if (redirect.size() == 2)
	{
		if (redirect[0] == "301") redirection_.code = 301;
		else if (redirect[0] == "302") redirection_.code = 302;
		else if (redirect[0] == "303") redirection_.code = 303;
		else if (redirect[0] == "307") redirection_.code = 307;
		else if (redirect[0] == "308") redirection_.code = 308;
		else throw std::invalid_argument("Invalid return redirection code: " + redirect[0]);

		redirection_.route = redirect[1];
		redirection_.enabled = true;
	}

	// ------------- AUTOINDEX ------------- //
	std::vector<std::string> autoindex;
	try {
		autoindex = directive_.get<std::vector<std::string> >("autoindex");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid autoindex value in location. Error: " + std::string(e.what()));
	}

	if (autoindex.size() < 1)
	{
		if (!parent_) autoindex_ = false;
		else autoindex_ = parent_->autoindex_;
	}
	else
	{
		std::string& el = autoindex.at(0);

		if (el == "on") autoindex_ = true;
		else if (el == "off") autoindex_ = false;
		else throw std::invalid_argument("Invalid autoindex value: " + el);
	}
	
	// --------------- INDEX --------------- //
	std::vector<std::string> index;
	try {
		index = directive_.get<std::vector<std::string> >("index");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid index value in location. Error: " + std::string(e.what()));
	}

	if (index.size()) index_ = index;
	else if (parent_) index_ = parent_->index_;

	// ------------ UPLOAD_PATH ------------ //
	std::vector<std::string> upload_path;
	try {
		upload_path = directive_.get<std::vector<std::string> >("upload_path");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid upload_path value in location. Error: " + std::string(e.what()));
	}

	if (upload_path.size())
	{
		upload_.enabled = true;
		upload_.path = upload_path.at(0);
	}
	else upload_.enabled = false;

	// -------------- CGI_EXT -------------- //
	std::vector<StrVecDirective> cgi_handler;
	try {
		cgi_handler = directive_.find<std::vector<std::string> >("cgi_handler");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid upload_path value in location. Error: " + std::string(e.what()));
	}

	if (cgi_handler.size())
	{
		cgi_.enabled = true;
		for (std::vector<StrVecDirective>::iterator it = cgi_handler.begin(); it != cgi_handler.end(); it++)
		{
			if (it->value.size() != 2) throw std::invalid_argument("Invalid cgi_handler value (2 arguments required)");
			cgi_.map[it->value.at(0)] = it->value.at(1);
		}
	}
	else cgi_.enabled = false;

	// ----------------------------------------------- //

	std::vector<StrDirective> childs = directive_.find<std::string>("location");

	childs_.reserve(childs.size());

	for (std::vector<StrDirective>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		childs_.push_back(Location(*it, this));
		Location& loc = childs_.back();
		loc.init();
	}

}

const std::string *Location::getErrorPage(int code) const
{
	if (!parent_) return NULL;

	const std::map<unsigned int, std::string>::const_iterator found_error_page = error_pages_.find(code);

	if (found_error_page != error_pages_.end())
		return &found_error_page->second;

	return parent_->getErrorPage(code);
}

const char *allow_methods_to_string(allow_methods_t m)
{
	static char buf[32];
	buf[0] = '\0';

	if (m & METHOD_GET)
		std::strcat(buf, "GET, ");
	if (m & METHOD_POST)
		std::strcat(buf, "POST, ");
	if (m & METHOD_DELETE)
		std::strcat(buf, "DELETE, ");

	if (buf[0] == '\0')
		return "NONE";

	buf[std::strlen(buf) - 2] = '\0';
	return buf;
}

void Location::print(int indent)
{
	if (parent_ == NULL) std::cout << PINK "Default location" RESET << std::endl;
	else std::cout << std::string(indent, ' ') << cfg::util::represent(route_) << std::endl;

	std::cout << std::string(indent, ' ') << " - root: " + root_ << std::endl;
	std::cout << std::string(indent, ' ') << " - client_max_body_size: " << client_max_body_size_ << std::endl;
	std::cout << std::string(indent, ' ') << " - autoindex: " << autoindex_ << std::endl;
	std::cout << std::string(indent, ' ') << " - index: " << cfg::util::represent(index_) << std::endl;
	if (redirection_.enabled) std::cout << std::string(indent, ' ') << " - redirection: " << redirection_.code << " " << redirection_.route << std::endl;
	if (upload_.enabled) std::cout << std::string(indent, ' ') << " - upload: " << upload_.path << std::endl;
	if (cgi_.enabled)
	{
		std::cout << std::string(indent, ' ') << " - cgi:" << std::endl;
		for (std::map<std::string, std::string>::iterator it = cgi_.map.begin(); it != cgi_.map.end(); it++)
			std::cout << std::string(indent, ' ') << "   • " << it->first << " -> " << it->second << std::endl;
	}

	for (std::vector<Location>::iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		it->print(indent + 2);
	}
}

Location::~Location()
{

}