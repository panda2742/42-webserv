#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

#include "config/HttpConfig.hpp"

#define DEFAULT_CLIENT_MAX_BODY_SIZE 20971520

enum LocationType {
	LOCATION_DEFAULT,
	LOCATION_REdIRECT,
	LOCATION_CGI,
	LOCATION_UPLOAD
};

class Location
{
private:
	StrDirective& directive_;

	std::vector<Location> childs_;
	Location* parent_;

	LocationType type_;
	std::vector<std::string> route_;

	std::string root_;
	std::map<unsigned int, std::string> error_pages_;
	unsigned long client_max_body_size_;

public:
	Location(StrDirective& directive, Location *parent);
	~Location();

	void init();
	
	const std::string *getErrorPage(int code) const; 

	void print(int indent = 0);
};

#endif
