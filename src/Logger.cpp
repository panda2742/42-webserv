
#include "Logger.hpp"

void Logger::log(Level lvl, const std::string &msg)
{
	std::ostream &out = (lvl == ERROR) ? std::cerr : std::cout;
	out << timestamp() << " " << levelToString(lvl) << " " << msg << std::endl;
}

std::string Logger::timestamp()
{
	std::time_t now = std::time(NULL);
	std::tm *tm = std::localtime(&now);

	char buffer[100];
	strftime(buffer, 99, "%Y/%m/%d %H:%M:%S", tm);
	return buffer;
}

std::string Logger::levelToString(Level lvl)
{
	switch (lvl)
	{
		case INFO:  return "\033[32m[info]\033[0m";
		case WARN:  return "\033[33m[warn]\033[0m";
		case ERROR: return "\033[31m[error]\033[0m";
	}
	return "[unknown]";
}
