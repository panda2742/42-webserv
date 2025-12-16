#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

#include <vector>
#include <iostream>
#include <sys/types.h>
#include <map>

/**
 * @enum Method
 * 
 * List of supported http methods
 */
enum Method {
	UNKNOWN,
	GET,
	POST,
	DELETE
};

/**
 * @enum RequestError
 * @brief Error codes encountered while constructing/parsing a request.
 *
 * - NO_REQ_ERROR : no error
 * - NOT_HTTP_HEADER : the received data does not start with an HTTP header
 * - UNKNOWN_METHOD : unknown HTTP method
 * - UNSUPPORTED_HTTP_VERSION : HTTP version not supported
 * - NO_HTTP_VERSION : no HTTP version found at all
 * - BAD_REQUEST : generic parsing error
 */
enum RequestError
{
	NO_REQ_ERROR,
	NOT_HTTP_HEADER,
	UNKNOWN_METHOD,
	UNSUPPORTED_HTTP_VERSION,
	NO_HTTP_VERSION,
	BAD_REQUEST,
	HTTPS_REQUEST
};

/**
 * @class HttpRequest
 * 
 * This class take a raw request, and parse it.
 * It will allow you to easily get the differents parts of the request :
 * headers, queries, and the body. The request can hold an error if there
 * is any issue with the parsing.
 */
class HttpRequest
{

private:

	std::vector<char> raw_;
	size_t header_size_;
	size_t content_size_;

	std::string first_line_;
	Method method_;
	std::string target_;
	std::string version_;

	RequestError create_error_;
	
	std::map<std::string, std::string> queries_;
	std::map<std::string, std::string> infos_;

	bool checkHttpVersion();
	bool parseTarget();

public:
	HttpRequest();
	~HttpRequest();

	/**
	 * @brief Initialize the HttpRequest object with raw received data.
	 * @param raw Buffer containing the full message (headers + body).
	 * @param header_size Size in bytes of the header portion within `raw`.
	 * @param content_size Size in bytes of the expected body.
	 *
	 * After `init` is called, invoke `parse()` to analyze the request line,
	 * headers and extract query parameters if present.
	 */
	void init(std::vector<char>& raw, size_t header_size, size_t content_size);

	/**
	 * @brief Parse the request stored in `raw_`.
	 * @return true if parsing succeeds and the request is valid.
	 *	Fills `method_`, `target_`, `version_`, `infos_` (headers) and `queries_`.
	 */
	bool parse();

	std::string getTarget() { return target_; }
	Method getMethod() { return method_; }
	size_t getContentSize() { return content_size_; }
	char* getBody() { return raw_.data() + header_size_; }
	RequestError getRequestError() { return create_error_; }
	std::string& getFirstLine() { return first_line_; }
	const std::string* getHeaderInfo(const std::string& key) const;
	const std::map<std::string, std::string>& getHeaders() const { return infos_; }
	const std::map<std::string, std::string>& getQueries() const { return queries_; }

	void clear();

};

#endif
