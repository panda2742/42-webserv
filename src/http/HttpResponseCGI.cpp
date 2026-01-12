#include "http/HttpResponse.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <algorithm>
#include "Server.hpp"
#include <fcntl.h>

void HttpResponse::sendBodyCGI()
{
	if (cgi_state_ != SEND_BODY) return ;

	size_t body_size = req_.getContentSize();

	if (body_size - send_index_ > 0)
	{
		ssize_t written = write(cgi_in_, req_.getBody() + send_index_, body_size - send_index_);

		if (written > 0)
		{
			send_index_ += written;
			return ;
		}
	}

	server_.removeCgiFd(cgi_in_);
	close(cgi_in_);
	cgi_in_ = -1;
	cgi_state_ = WAIT_CONTENT;
}

void HttpResponse::getContentCGI()
{
	if (cgi_state_ != WAIT_CONTENT) return ;

	char buf[4096];

	ssize_t n = read(cgi_out_, buf, sizeof(buf));
	if (n > 0)
	{
		body_.insert(body_.end(), buf, buf + n);
	}
	else
	{
		server_.removeCgiFd(cgi_out_);
		close(cgi_out_);
		cgi_out_ = -1;

		handleResultCGI();

		cgi_state_ = CGI_FINISHED;
	}
	
}


void HttpResponse::useCGI(const std::string& cgi_prog, const std::string& script_path)
{
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) < 0)
	{
		setError(500);
		return ;
	}
	if (pipe(pipe_out) < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		setError(500);
		return ;
	}
	
	pid_t pid = fork();
	if (pid < 0)
	{
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		setError(500);
		return ;
	}
	else if (pid == 0)
	{
		close(pipe_in[1]);
		close(pipe_out[0]);
		if (dup2(pipe_out[1], STDOUT_FILENO) < 0
			|| dup2(pipe_in[0], STDIN_FILENO) < 0)
		{
			close(pipe_in[0]);
			close(pipe_out[1]);
			throw std::runtime_error("CGI failed (dup2 failed)");
		}
		close(pipe_in[0]);
		close(pipe_out[1]);

		execChildCGI(cgi_prog, script_path);
	}

	close(pipe_in[0]);
	close(pipe_out[1]);

	cgi_in_ = pipe_in[1];
	fcntl(cgi_in_, F_SETFL, O_NONBLOCK);
	fd_context_in_.type = CGI_IN;
	fd_context_in_.cgi_owner_response = this;
	server_.addCgiInFd(cgi_in_, &fd_context_in_);
	cgi_out_ = pipe_out[0];
	fcntl(cgi_out_, F_SETFL, O_NONBLOCK);
	fd_context_out_.type = CGI_OUT;
	fd_context_out_.cgi_owner_response = this;
	server_.addCgiOutFd(cgi_out_, &fd_context_out_);

	cgi_state_ = SEND_BODY;
	send_index_ = 0;
	waiting_cgi_ = true;
}

void HttpResponse::execChildCGI(const std::string& cgi_prog, const std::string& script_path)
{
	std::vector<std::string> env_strings;
	
	// SERVER
	env_strings.push_back("SERVER_SOFTWARE=webserv/"+ std::string(VERSION));
	const std::string* host = req_.getHeaderInfo("Host");
	if (host) env_strings.push_back("SERVER_NAME=" + *host);
	else env_strings.push_back("SERVER_NAME=");
	env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
	
	// Request
	env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_strings.push_back("SERVER_PORT=" + to_string(req_.getSocketContext()->port));
	env_strings.push_back("REQUEST_METHOD=" + getMethodName(req_.getMethod()));
	env_strings.push_back("PATH_INFO=" + path_info_);
	env_strings.push_back("SCRIPT_FILENAME=" + script_path);
	env_strings.push_back("SCRIPT_NAME=" + script_path.substr(script_path.find_last_of('/') + 1));
	std::string queries_str = "";
	for (std::map<std::string, std::string>::const_iterator it = req_.getQueries().begin(); it != req_.getQueries().end(); ++it)
	{
		if (it == req_.getQueries().begin()) queries_str += it->first + "=" + it->second;
		else queries_str += "&"+ it->first +"="+ it->second;
	}
	env_strings.push_back("QUERY_STRING="+ queries_str);

	struct in_addr ipaddr = req_.getConnectionContext()->ip;

	env_strings.push_back("REMOTE_ADDR="  + to_string(int(ipaddr.s_addr&0xFF))
									+ "." + to_string(int((ipaddr.s_addr&0xFF00)>>8)) 
									+ "." + to_string(int((ipaddr.s_addr&0xFF0000)>>16))
									+ "." + to_string(int((ipaddr.s_addr&0xFF000000)>>24)));
	

	if (req_.getHeaderInfo("Content-Type"))
		env_strings.push_back("CONTENT_TYPE=" + *req_.getHeaderInfo("Content-Type"));
	env_strings.push_back("CONTENT_LENGTH=" + to_string(req_.getContentSize()));
	env_strings.push_back("REDIRECT_STATUS=200");

	// HTTP
	const std::map<std::string, std::string>& headers = req_.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		it != headers.end();
		++it)
	{
		env_strings.push_back("HTTP_"+ toUpper(it->first) +"="+ it->second);
	}

	std::vector<char*> envp;
	for (size_t i = 0; i < env_strings.size(); ++i)
		envp.push_back(const_cast<char*>(env_strings[i].c_str()));
	envp.push_back(NULL);

	std::vector<char*> args;
	args.push_back(const_cast<char*>(cgi_prog.c_str()));
	args.push_back(const_cast<char*>(script_path.c_str()));
	args.push_back(NULL);

	server_.clean();
	server_.setChild();

	execve(cgi_prog.c_str(), args.data(), envp.data());
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	throw std::runtime_error("CGI execve failed");
}

void HttpResponse::handleResultCGI()
{
	setStatus(200);

	Logger::info("CGI out handling");
	body_.push_back('\0');

	static const std::string sep = "\r\n\r\n";

	std::vector<char>::iterator it = std::search(
		body_.begin(), body_.begin() + body_.size(),
		sep.begin(), sep.end()
	);

	if (it == body_.begin() + body_.size())
	{
		body_.clear();
		setError(500);
		waiting_cgi_ = false;
		serializeHeader();
		return ;
	}

	size_t header_len = it - body_.begin();

	std::string header_str(body_.begin(), body_.begin() + header_len);

	body_.erase(body_.begin(), it + 2);

	size_t start = 0;
	while (true)
	{
		size_t end = header_str.find("\r\n", start);
		std::string line;
		if (end == std::string::npos) line = header_str.substr(start);
		else line = header_str.substr(start, end - start);

		if (!line.empty())
		{
			size_t colon = line.find(':');
			if (colon != std::string::npos)
			{
				std::string key = trim(line.substr(0, colon));
				std::string val = trim(line.substr(colon + 1));
				if (!key.empty())
				{
					if (key == "Status")
					{
						size_t space = val.find(' ');
						if (space != std::string::npos)
							setStatus(std::atoi(val.substr(0, space).c_str()), trim(val.substr(space + 1)));
					}
					headers_[key] = val;
				}
			}
		}

		if (end == std::string::npos)
			break;
		start = end + 2;
	}

	headers_["Content-Length"] = to_string(body_.size());
	waiting_cgi_ = false;
	serializeHeader();
}

