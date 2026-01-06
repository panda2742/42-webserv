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

	/**
	 * An utility function that returns true if two vectors are identical in their content.
	 *
	 * @tparam T The type of the vector elements. T MUST have implemented the assignation operator.
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @return The result of the test which is a boolean.
	 */
	template <typename T>
	static bool	vecCmp_(const std::vector<T>& vec1, const std::vector<T>& vec2);

	/**
	 * Process the matches function but private because what do you mean CIA can spy me?
	 */

public:
	Location(StrDirective& directive, Location *parent);
	~Location();

	void init();

	void print(int indent = 0);

	/**
	 * Take a list of fragments as a parameter. Return a reference to the matching location based
	 * on those fragments. Fragments are the splitted path of the URL.
	 *
	 * @param fragments The sequence of fragments.
	 * @return A reference to the found location.
	 */
	const Location&	matches(const std::vector<std::string>& fragments = std::vector<std::string>()) const;
};

#include "Location.tpp"

#endif
