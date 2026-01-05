#ifndef __LOCATION_HPP__
# define __LOCATION_HPP__

#include "config/HttpConfig.hpp"

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

public:
	Location(StrDirective& directive, Location *parent);
	~Location();

	void init();
};

#endif
