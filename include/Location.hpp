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

	template <typename T>
	static bool	vecCmp_(std::vector<T>& vec1, std::vector<T>& vec2);

	template <typename T>
	static std::vector<T>	vecConsume_(const std::vector<T>& vec1, const std::vector<T>& vec2);

	struct MatchRes
	{
		Location	*res;
		size_t		common;

		MatchRes&	nullify(void) { this->res = NULL; return *this; }
		MatchRes&	spotify(Location *loc) { this->res = loc; return *this; }
		MatchRes(void): res(NULL), common(0) {}
		MatchRes(const MatchRes& other): res(other.res), common(other.common) {}
		MatchRes&	operator=(const MatchRes& other)
		{
			if (this != &other)
			{
				this->common = other.common;
				this->res = other.res;
			}
			return *this;
		}
	};
public:
	Location(StrDirective& directive, Location *parent);
	~Location();

	void init();

	void print(int indent = 0) const;

	MatchRes	matchescanbenullbecauseitsapointer(vecstr_t fragments, MatchRes res);
	Location&	matches(vecstr_t fragments);
};

#include "Location.tpp"

#endif
