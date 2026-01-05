#include "Location.hpp"
#include "utils.hpp"

Location::Location(StrDirective& directive, Location *location) : directive_(directive), parent_(location), type_(LOCATION_DEFAULT)
{
	
}

void Location::init()
{

	std::vector<StrDirective> childs = directive_.find<std::string>("location");

	for (std::vector<StrDirective>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		childs_.push_back(Location(*it, this));
		Location& loc = childs_.back();
		loc.init();
	}

	// ----------------------------------------------- //

	route_ = split(directive_.value, '/');


}

Location::~Location()
{

}