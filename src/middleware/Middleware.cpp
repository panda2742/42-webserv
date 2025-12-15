#include "middleware/Middleware.hpp"

namespace mdw
{
// #########################################################
/**
 * Represent the class that does all the tests for the middleware operations between the configuration parsing and
 * the use in the server part of the project.
 */

Middleware::Middleware(cfg::HttpConfig& conf): conf_(conf) {}

Middleware::~Middleware(void) {}

// #########################################################
};