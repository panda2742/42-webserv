#ifndef MIDDLEWARE_HPP
#define MIDDLEWARE_HPP

#include "config/HttpConfig.hpp"

namespace mdw
{
// #########################################################

/**
 * Represent an exception during the middleware process.
 */
class MiddlewareException : public std::runtime_error
{
	public:
	MiddlewareException(const std::string& msg);
};

/**
 * Represent the class that does all the tests for the middleware operations between the configuration parsing and
 * the use in the server part of the project.
 */
class Middleware
{
	public:
	/**
	 * @param conf The HttpConfig instance to do the middleware tests onto.
	 */
	Middleware(cfg::HttpConfig& conf);
	~Middleware(void);

	/**
	 * Check the format and ensures there is the correct amount of brackets, and other formating stuff.
	 */
	bool	checkFormat(const std::vector<cfg::Lexer::TokenNode>& nodes) const throw(MiddlewareException);

	private:
	/**
	 * The instance of the HttpConfig class to do the test onto.
	 */
	cfg::HttpConfig&	conf_;
};

// #########################################################
};

#endif