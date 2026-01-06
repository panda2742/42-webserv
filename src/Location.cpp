#include "Location.hpp"
#include "utils.hpp"

Location::Location(StrDirective& directive, Location *location) : directive_(directive), parent_(location), type_(LOCATION_DEFAULT)
{
}

void Location::init()
{
	route_ = split(directive_.value, '/');

	try {
		std::vector<std::string> root_vec = directive_.get<std::vector<std::string> >("root");
		if (root_vec.size() < 1) {
			root_ = parent_->root_;
			for (std::vector<std::string>::const_iterator rit(route_.begin()); rit != route_.end(); rit++) {
				root_ += '/' + *rit;
			}
		} else {
			root_ = root_vec.at(0);
		}

		if (root_.empty()) throw std::invalid_argument("root is required");

		if (root_.size() - 1 == '/') root_.erase(root_.end() - 1);
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid root value in location. Error: " + std::string(e.what()));
	}

	try {
		error_pages_ = directive_.get<std::map<unsigned int, std::string> >("error_page");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid error_page value in location. Error: " + std::string(e.what()));
	}

	try {
		error_pages_ = directive_.get<std::map<unsigned int, std::string> >("error_page");
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid error_page value in location. Error: " + std::string(e.what()));
	}

	// ----------------------------------------------- //

	std::vector<StrDirective> childs = directive_.find<std::string>("location");

	for (std::vector<StrDirective>::iterator it = childs.begin(); it != childs.end(); ++it)
	{
		childs_.push_back(Location(*it, this));
		Location& loc = childs_.back();
		loc.init();
	}

}

const std::string *Location::getErrorPage(int code) const {
	if (!parent_) return NULL;
	const std::map<unsigned int, std::string>::const_iterator found_error_page = error_pages_.find(code);
	if (found_error_page != error_pages_.end())
		return &found_error_page->second;
	return parent_->getErrorPage(code);
}

void Location::print(int indent)
{
	if (parent_ == NULL) std::cout << PINK "Default location" RESET << std::endl;
	else std::cout << std::string(indent, ' ') << cfg::util::represent(route_) << std::endl;

	std::cout << std::string(indent, ' ') << " - root: " + root_ << std::endl;

	for (std::vector<Location>::iterator it = childs_.begin(); it != childs_.end(); ++it)
	{
		it->print(indent + 2);
	}
}

Location::~Location()
{

}