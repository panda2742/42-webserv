#ifndef CONFIG_LOGGER_HPP
#define CONFIG_LOGGER_HPP

#include "types.hpp"
#include <string>
#include <ostream>
#include <iostream>

#define INFO_PRFX "["CYAN" INFO  "RESET"]    "
#define WARN_PRFX "["ORANGE" WARN  "RESET"]    "
#define DEBUG_PRFX "["BLURPLE" DEBUG "RESET"]    "
#define ERROR_PRFX "["RED" ERROR "RESET"]    "

namespace Config
{
// #########################################################

class ConfigLogger
{
	private:
	void	printMsg_(const std::string& msg, std::ostream& os)
	{
		os << "CONFIG *    " << msg << std::endl;
		std::flush(os);
	}

	public:
	ConfigLogger(void) {}
	~ConfigLogger(void) {}

	void	info(const std::string& msg) { printMsg_(std::string(INFO_PRFX) + msg, std::cout); }
	void	warn(const std::string& msg) { printMsg_(std::string(WARN_PRFX) + msg, std::cout); }
	void	debug(const std::string& msg) { printMsg_(std::string(DEBUG_PRFX) + msg, std::cout); }
	void	error(const std::string& msg) { printMsg_(std::string(ERROR_PRFX) + msg, std::cerr); }

	void	info(const char *cstr) { info(std::string(cstr)); }
	void	warn(const char *cstr) { warn(std::string(cstr)); }
	void	debug(const char *cstr) { debug(std::string(cstr)); }
	void	error(const char *cstr) { error(std::string(cstr)); }
};

extern ConfigLogger	log;

// #########################################################
};

#endif