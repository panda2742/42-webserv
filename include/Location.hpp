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

	const std::string *getErrorPage(int code) const;

	void print(int indent = 0) const;

	MatchRes	matchescanbenullbecauseitsapointer(vecstr_t fragments, MatchRes res);
	Location&	matches(vecstr_t fragments);

	unsigned long getClientMaxBodySize() const { return client_max_body_size_; }
};

#include "Location.tpp"

#endif
