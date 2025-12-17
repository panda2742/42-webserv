#include "config/ConfigLogger.hpp"

namespace cfg
{
// #########################################################

void	ConfigLogger::printMsg_(const str_t& msg, std::ostream& os)
{
	os << "CONFIG *    " << msg << std::endl;
	std::flush(os);
}

ConfigLogger::ConfigLogger(void) {}
ConfigLogger::~ConfigLogger(void) {}

void	ConfigLogger::info(const str_t& msg)
{
	printMsg_(str_t(INFO_PRFX) + msg, std::cout);
}

void	ConfigLogger::warn(const str_t& msg)
{
	printMsg_(str_t(WARN_PRFX) + msg, std::cout);
}

void	ConfigLogger::debug(const str_t& msg)
{
	printMsg_(str_t(DEBUG_PRFX) + msg, std::cout);
}

void	ConfigLogger::error(const str_t& msg)
{
	printMsg_(str_t(ERROR_PRFX) + msg, std::cerr);
}

void	ConfigLogger::info(const char *cstr)
{
	info(str_t(cstr));
}

void	ConfigLogger::warn(const char *cstr)
{
	warn(str_t(cstr));
}

void	ConfigLogger::debug(const char *cstr)
{
	debug(str_t(cstr));
}

void	ConfigLogger::error(const char *cstr)
{
	error(str_t(cstr));
}

ConfigLogger	log;

// #########################################################
};