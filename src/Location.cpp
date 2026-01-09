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

Location::MatchRes	Location::matchescanbenullbecauseitsapointer(vecstr_t fragments, MatchRes res = MatchRes())
{
	std::vector<std::string>	consumed = vecConsume_(fragments, route_);
	if (consumed.size()) return res;

	res.common += route_.size();

	if (parent_ != NULL) ++res.common;

	res.spotify(this);

	std::vector<std::string> remaining_fragments = fragments;
	remaining_fragments.erase(remaining_fragments.begin(), remaining_fragments.begin() + route_.size());

	MatchRes	a_son_prime(res);
	for (std::vector<Location>::iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		MatchRes	child_res = it->matchescanbenullbecauseitsapointer(remaining_fragments, res);
		if (child_res.common > a_son_prime.common)
			a_son_prime = child_res;
	}

	if (a_son_prime.common > res.common)
		return a_son_prime;
	return res;
}

Location& Location::matches(vecstr_t fragments)
{
	Location	*res = matchescanbenullbecauseitsapointer(fragments).res;
	if (res)
		return *res;
	return *this;
}

Location::~Location()
{

}
