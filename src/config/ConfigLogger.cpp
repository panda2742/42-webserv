#include "config/ConfigLogger.hpp"

namespace Config
{
// #########################################################

void	ConfigLogger::printMsg_(const std::string& msg, std::ostream& os)
{
	os << "CONFIG *    " << msg << std::endl;
	std::flush(os);
}

ConfigLogger::ConfigLogger(void) {}
ConfigLogger::~ConfigLogger(void) {}

void	ConfigLogger::info(const std::string& msg)
{
	printMsg_(std::string(INFO_PRFX) + msg, std::cout);
}

void	ConfigLogger::warn(const std::string& msg)
{
	printMsg_(std::string(WARN_PRFX) + msg, std::cout);
}

void	ConfigLogger::debug(const std::string& msg)
{
	printMsg_(std::string(DEBUG_PRFX) + msg, std::cout);
}

void	ConfigLogger::error(const std::string& msg)
{
	printMsg_(std::string(ERROR_PRFX) + msg, std::cerr);
}

void	ConfigLogger::info(const char *cstr)
{
	info(std::string(cstr));
}

void	ConfigLogger::warn(const char *cstr)
{
	warn(std::string(cstr));
}

void	ConfigLogger::debug(const char *cstr)
{
	debug(std::string(cstr));
}

void	ConfigLogger::error(const char *cstr)
{
	error(std::string(cstr));
}

ConfigLogger	log;

// #########################################################
};