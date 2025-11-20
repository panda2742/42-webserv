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

const std::string getHttpErrorMessage(int code);
const std::string getMimeType(const std::string& ext);

const std::string respStateToText(ResponseState s);
