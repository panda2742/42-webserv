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

void Location::print(int indent) const
{
	if (parent_ == NULL) std::cout << "Default location" << std::endl;
	else std::cout << std::string(indent, ' ') << cfg::util::represent(route_)  << " ";
	std::cout << this << std::endl;

	for (std::vector<Location>::const_iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		it->print(indent + 2);
	}
}

Location* Location::matchProcess_(vecstr_t& fragments, Location& location)
{
	for (std::vector<Location>::iterator it = location.childs_.begin(); it != location.childs_.end(); ++it)
	{
		Location* loc_child = matchProcess_(fragments, *it);
		if (loc_child)
			return loc_child;
	}

	vecstr_t	remaining = vecConsume_(fragments, location.route_);

	if (remaining.size() < fragments.size()) return &location;

	return NULL;
}

Location* Location::matches(vecstr_t fragments)
{
	for (std::vector<Location>::iterator it = childs_.begin();
		it != childs_.end(); ++it)
	{
		Location* match = matchProcess_(fragments, *it);
		if (match)
			return match;
	}

	return NULL;  // Ou this si vous voulez un fallback
}

Location::~Location()
{

}
