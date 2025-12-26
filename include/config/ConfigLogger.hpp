#ifndef CONFIG_LOGGER_HPP
#define CONFIG_LOGGER_HPP

#include "types.hpp"
#include <string>
#include <ostream>
#include <iostream>
#include "global.hpp"

#define INFO_PRFX "[" CYAN " INFO  " RESET "]    "
#define WARN_PRFX "[" ORANGE " WARN  " RESET "]    "
#define DEBUG_PRFX "[" BLURPLE " DEBUG " RESET "]    "
#define ERROR_PRFX "[" RED " ERROR " RESET "]    "

namespace cfg
{
// #########################################################

class ConfigLogger
{
	private:
	/**
	 * Print a message into the specified output stream.
	 *
	 * @param msg The message to print.
	 * @param os The output stream.
	 */
	void	printMsg_(const str_t& msg, std::ostream& os);

	public:
	ConfigLogger(void);
	~ConfigLogger(void);

	/**
	 * Cyan colored message to the output stream.
	 *
	 * @param msg The message to print.
	 */
	void	info(const str_t& msg);

	/**
	 * Orange colored message to the output stream.
	 *
	 * @param msg The message to print.
	 */
	void	warn(const str_t& msg);

	/**
	 * Blurple colored message to the output stream.
	 *
	 * @param msg The message to print.
	 */
	void	debug(const str_t& msg);

	/**
	 * Red colored message to the error stream.
	 *
	 * @param msg The message to print.
	 */
	void	error(const str_t& msg);

	/**
	 * Cyan colored message to the output stream.
	 *
	 * @param msg The message to print.
	 */
	void	info(const char *cstr);

	/**
	 * Orange colored message to the output stream.
	 *
	 * @param msg The message to print.
	 */
	void	warn(const char *cstr);

	/**
	 * Blurple colored message to the output stream.
	 *
	 * @param msg The message to print.
	 */
	void	debug(const char *cstr);

	/**
	 * Red colored message to the error stream.
	 *
	 * @param msg The message to print.
	 */
	void	error(const char *cstr);
};

extern ConfigLogger	log;

// #########################################################
};

#endif