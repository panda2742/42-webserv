#ifndef __LOGGER_HPP__
# define __LOGGER_HPP__

#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

class Logger
{

public:
	enum Level {
		INFO,
		WARN,
		ERROR
	};

	static void	log(Level lvl, const std::string &msg);

	static void	info(const std::string &msg) { log(INFO, msg); }
	static void	warn(const std::string &msg) { log(WARN, msg); }
	static void	error(const std::string &msg) { log(ERROR, msg); }

private:
	static std::string	timestamp(void);

	static std::string	levelToString(Level lvl);
};

#endif
