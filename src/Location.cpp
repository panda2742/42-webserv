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

void Location::print(int indent)
{
	if (parent_ == NULL) std::cout << "Default location" << std::endl;
	else std::cout << std::string(indent, ' ') << cfg::util::represent(route_) << std::endl;

	for (std::vector<Location>::iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		it->print(indent + 2);
	}
}

const Location&	Location::matches(const vecstr_t& fragments) const
{
	if (!fragments.size() || vecCmp_(route_, fragments)) return *this;

	for (std::vector<Location>::const_iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		
	}

	return *this;
}

Location::~Location()
{

}
