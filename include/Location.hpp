#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

#include "config/HttpConfig.hpp"

#include <stdint.h>

#define DEFAULT_CLIENT_MAX_BODY_SIZE 20971520

enum LocationType {
	LOCATION_DEFAULT,
	LOCATION_REdIRECT,
	LOCATION_CGI,
	LOCATION_UPLOAD
};

typedef uint8_t allow_methods_t;

#define METHOD_GET		(1 << 0)
#define METHOD_POST		(1 << 1)
#define METHOD_DELETE	(1 << 2)

typedef struct {
	bool enabled;
	int code;
	std::string route;
} redirect_t;

typedef struct {
	bool enabled;
	std::string path;
} upload_t;

typedef struct {
	bool enabled;
	std::map<std::string, std::string> map;
} cgi_t;

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
	allow_methods_t allow_methods_;
	redirect_t redirection_;
	bool autoindex_;
	std::vector<std::string> index_;
	upload_t upload_;
	cgi_t cgi_;

public:
	Location(StrDirective& directive, Location *parent);
	~Location();

	void init();
	
	const std::string *getErrorPage(int code) const; 

	void print(int indent = 0);
};

#endif
