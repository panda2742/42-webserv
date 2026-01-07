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
	 * @tparam T The type of the vector elements. T MUST have implemented the equal operator.
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @return The result of the test which is a boolean.
	 */
	template <typename T>
	static bool	vecCmp_(std::vector<T>& vec1, std::vector<T>& vec2);

	template <typename T>
	static std::vector<T> vecConsume_(const std::vector<T>& vec1, const std::vector<T>& vec2);

	/**
	 * Process the matches function but private because what do you mean CIA can spy me?
	 *
	 * @param fragments The sequence of fragments.
	 * @param location The location to process.
	 * @return A pointer to the location if it matches or NULL if it does not.
	 */
	Location	*matchProcess_(vecstr_t& fragments, Location& location);

public:
	Location(StrDirective& directive, Location *parent);
	~Location();

	void init();

	void print(int indent = 0) const;

	Location* matches(vecstr_t fragments);
};

#include "Location.tpp"

#endif
