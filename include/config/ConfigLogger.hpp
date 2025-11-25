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
	void	printMsg_(const std::string& msg, std::ostream& os);

	public:
	ConfigLogger(void);
	~ConfigLogger(void);

	void	info(const std::string& msg);
	void	warn(const std::string& msg);
	void	debug(const std::string& msg);
	void	error(const std::string& msg);

	void	info(const char *cstr);
	void	warn(const char *cstr);
	void	debug(const char *cstr);
	void	error(const char *cstr);
};

extern ConfigLogger	log;

// #########################################################
};

#endif