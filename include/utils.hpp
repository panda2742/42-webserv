#include <sstream>
#include <string>
#include "HttpResponse.hpp"

template<typename T>
std::string to_string(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string toUpper(const std::string& str);
std::string getNextPart(std::string& input, const std::string& sep);
std::string trim(const std::string &s);

const std::string getHttpErrorMessage(int code);
const std::string getMimeType(const std::string& ext);
const std::string getMethodName(Method m);

const std::string respStateToText(ResponseState s);
