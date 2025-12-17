#ifndef MIDDLEWARE_HPP
#define MIDDLEWARE_HPP

#include "config/HttpConfig.hpp"

namespace mdw
{
// #########################################################
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
	 * 
	 */

	private:
	/**
	 * The instance of the HttpConfig class to do the test onto.
	 */
	cfg::HttpConfig&	conf_;
};

// #########################################################
};

#endif