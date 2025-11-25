#include "HttpResponse.hpp"
#include "FileCacheManager.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <algorithm>

void HttpResponse::setStatus(int code, const std::string &message) {
	status_code_ = code;
	status_message_ = message;
}

void HttpResponse::setHeader(const std::string &name, const std::string &value) {
	headers_[name] = value;
}

void HttpResponse::setBody(const std::vector<char> &body) {
	body_ = body;
	headers_["Content-Length"] = to_string(body.size());
}

void HttpResponse::setError(int code)
{
	setStatus(code, getHttpErrorMessage(code));
	
	// try load configurated error page
	// + set file status a FILE_OK si y'a bien un fichier / FILE_STREAM_DIRECT si trop lourd

	setHeader("Content-Type", "text/html");
	std::string error = to_string(code) + " " + status_message_;
	std::string body = "<html><head><title>"+error+"</title></head><body><center><h1>"+error+"</h1></center><hr><center>webserv/"+std::string(VERSION)+"</center></body></html>";
	std::vector<char> body_vec(body.begin(), body.end());
	setBody(body_vec);
}

void HttpResponse::setDirectory()
{
	struct stat tmp_file_info;

	std::string full_path_tmp;
	std::string index_path = req_.getTarget()[req_.getTarget().size() - 1] == '/' ? req_.getTarget() + "index.html" : req_.getTarget() + "/index.html";

	FileStatus index_status = FileCacheManager::getFile(index_path, file_, tmp_file_info, full_path_tmp);


	if (index_status == FILE_OK || index_status == FILE_STREAM_DIRECT)
	{
		file_path_ = full_path_tmp;
		file_info_ = tmp_file_info;
		file_status_ = index_status;
		createDefault();
		return ;
	}

	std::string dirname = (req_.getTarget()[req_.getTarget().size() - 1] == '/') ? req_.getTarget() : (req_.getTarget() + "/");
	std::string auto_index_html = "<!DOCTYPE html><html><head><title>Index of "+ dirname + "</title></head><body><h1>Index of "+ dirname + "</h1><hr><pre>";


	DIR *dr;
	struct dirent *en;
	dr = opendir(file_path_.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
		{
			std::string filename = en->d_name;

			if (filename == ".") continue ;

			if (en->d_type == DT_DIR) filename += "/";

			auto_index_html += "<a href=\""+ dirname + filename +"\">"+ filename +"</a><br>";
		}
		closedir(dr);
	}

	auto_index_html += "</pre><hr></body></html>";

	setStatus(200, "OK");
	setHeader("Content-Type", "text/html");
	std::vector<char> body_vec(auto_index_html.begin(), auto_index_html.end());
	setBody(body_vec);
}

void HttpResponse::setRedirect(int code, const std::string& target)
{
	setStatus(code, getHttpErrorMessage(code));
	headers_["Location"] = target;
}

void HttpResponse::addCookie(const std::string& name,
							const std::string& value,
							bool httpOnly = false,
							bool secure = false,
							ssize_t maxAge = -1,
							const std::string& path = "",
							const std::string& sameSite = "")
{
	cookies_.push_back((ResCookie){name, value, httpOnly, secure, maxAge, path, sameSite});
}

static inline std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
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
		dup2(pipe_out[1], STDOUT_FILENO); // GERER LES ERREURS
		dup2(pipe_in[0], STDIN_FILENO);

		std::vector<std::string> env_strings;
		
		// SERVER
		env_strings.push_back("SERVER_SOFTWARE=webserv/"+ std::string(VERSION));
		const std::string* host = req_.getHeaderInfo("Host");
		if (host) env_strings.push_back("SERVER_NAME=" + *host);
		else env_strings.push_back("SERVER_NAME=");
		env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		
		// Request
		env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		env_strings.push_back("SERVER_PORT="); // A FAIRE mais flemme maintenant
		env_strings.push_back("REQUEST_METHOD=" + getMethodName(req_.getMethod()));
		env_strings.push_back("PATH_INFO="); // A FAIRE mais infernal, genre faut tester la route / par / pour verifier si y'a pas une cgi avant et que c'est pas un path apres genre
		// env_strings.push_back("PATH_TRANSLATED="); // En vrai belek
		env_strings.push_back("SCRIPT_FILENAME=" + script_path);
		env_strings.push_back("SCRIPT_NAME=" + script_path.substr(script_path.find_last_of('/') + 1));
		const std::map<std::string, std::string> queries;
		std::string queries_str = "";
		for (std::map<std::string, std::string>::const_iterator it = queries.begin(); it != queries.end(); ++it)
		{
			if (it == queries.begin()) queries_str += it->first + "=" + it->second;
			else queries_str += "&"+ it->first +"="+ it->second;
		}
		env_strings.push_back("QUERY_STRING="+ queries_str);
		env_strings.push_back("REMOTE_ADDR="); // Faut que je stoque l'ip du client jsp comment
		// env_strings.push_back("AUTH_TYPE="); // Basic/Digest, pas compris a quoi ca sert
		// env_strings.push_back("REMOTE_USER="); // en rapport avec l'auth je crois
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
			std::cout << it->first << ": " << it->second << std::endl;
		}

		std::vector<char*> envp;
		for (size_t i = 0; i < env_strings.size(); ++i)
			envp.push_back(const_cast<char*>(env_strings[i].c_str()));
		envp.push_back(NULL);

		std::vector<char*> args;
		envp.push_back(const_cast<char*>(cgi_prog.c_str()));
		envp.push_back(const_cast<char*>(script_path.c_str()));
		envp.push_back(NULL);

		execve(cgi_prog.c_str(), args.data(), envp.data());
		std::exit(1);
	}

	close(pipe_in[0]);
	close(pipe_out[1]);

	close(pipe_in[1]); // ECRIRE DEDANS LE BODY DE LA REQ

	char buf[4096];
	ssize_t n;
	while ((n = read(pipe_out[0], buf, sizeof(buf))) > 0) {
		body_.insert(body_.end(), buf, buf + n);
	}
	close(pipe_out[0]);

	setStatus(200, "OK");

	static const std::string sep = "\r\n\r\n";

	std::vector<char>::iterator it = std::search(
		body_.begin(), body_.begin() + body_.size(),
		sep.begin(), sep.end()
	);

	if (it == body_.begin() + body_.size())
	{
		body_.clear();
		setError(500);
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
	
}

void HttpResponse::serializeHeader()
{
	serialized_header_ = "HTTP/1.1 " + to_string(status_code_) + " " + status_message_ + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
		serialized_header_ += it->first + ": " + it->second + "\r\n";

	for (std::vector<ResCookie>::const_iterator it = cookies_.begin(); it != cookies_.end(); ++it)
	{
		std::string cookie = "Set-Cookie: "+ it->name +"="+ it->value;

		if (it->httpOnly) cookie += "; HttpOnly";
		if (it->secure) cookie += "; Secure";
		if (it->maxAge > 0) cookie += "; Max-Age=" + to_string(it->maxAge);
		if (!it->path.empty()) cookie += "; Path=" + it->path;
		if (!it->sameSite.empty()) cookie += "; SameSite=" + it->sameSite;

		serialized_header_ += cookie + "\r\n";
	}

	serialized_header_ += "\r\n";
}

const std::string HttpResponse::getBodySize() const
{
	std::map<std::string, std::string>::const_iterator it = headers_.find("Content-Length");
	if (it == headers_.end()) return "0";
	return it->second;
}

void HttpResponse::createDefault()
{
	headers_["Content-Length"] = "0";

	// addCookie("test", "kakoukakou");
	// addCookie("test2", "kakoukakou2", true, true, 3600, "/", "Lax");

	useCGI("/usr/bin/php-cgi", "/home/lilefebv/Documents/cursus/42-webserv/www/script.php");
	return ;

	// if (req_.getTarget() == "/abc")
	// {
	// 	setRedirect(302, "/OEOEOEOE");
	// 	return ;
	// } // Redirection example
	if (req_.getMethod() == GET)
	{
		if (file_status_ == NONE) file_status_ = FileCacheManager::getFile(req_.getTarget(), file_, file_info_, file_path_);
		
		if (file_status_ == FILE_OK)
		{
			setStatus(200, "OK");
			setHeader("Content-Type", file_->mime);
			headers_["Content-Length"] = to_string(file_->size);
		}
		else if (file_status_ == FILE_STREAM_DIRECT)
		{
			direct_file_fd_ = open(file_path_.c_str(), O_RDONLY);
			
			if (direct_file_fd_ < 0)
				setError(500);
			else
			{
				setStatus(200, "OK");
				setHeader("Content-Type", getMimeType(getExtension(req_.getTarget())));
				headers_["Content-Length"] = to_string(file_info_.st_size);
			}
		}
		else if (file_status_ == FILE_IS_DIR) setDirectory();
		else if (file_status_ == PATH_FORBIDDEN) setError(400);
		else if (file_status_ == FILE_NOT_FOUND) setError(404);
		else if (file_status_ == FILE_FORBIDDEN) setError(403);
		else if (file_status_ == PATH_TO_LONG) setError(414);
		else setError(500);

	}
	else
	{
		setHeader("Content-Type", "text/plain");
		std::string body = "Unknown method";
		std::vector<char> body_vec(body.begin(), body.end());
		setBody(body_vec);
	}
}

void HttpResponse::create()
{
	switch (req_.getRequestError())
	{
	case NO_REQ_ERROR:
	case NO_HTTP_VERSION:
		createDefault(); break;
	case NOT_HTTP_HEADER:
		return ;
	case UNKNOWN_METHOD:
		setError(405); break;
	case UNSUPPORTED_HTTP_VERSION:
		setError(505); break;
	case BAD_REQUEST:
	default:
		setError(400); break;
	}

	if (res_ready_) return ;
	serializeHeader();
	res_ready_ = true;
	Logger::info("\"" + req_.getFirstLine() + "\" " + to_string(status_code_) + " " + getBodySize());
}

bool HttpResponse::sendFileDirectPart(int socket_fd)
{
	if (send_index_ > 0)
	{
		if (send_index_ < (size_t)direct_file_n_)
		{
			ssize_t s = send(socket_fd, direct_file_buffer_ + send_index_, direct_file_n_ - send_index_, 0);
			if (s <= 0) return false;

			send_index_ += s;

			return true;
		}
		else
			send_index_ = 0;
	}
	
	direct_file_n_ = read(direct_file_fd_, direct_file_buffer_, sizeof(direct_file_buffer_));
	if (direct_file_n_ > 0)
	{
		ssize_t s = send(socket_fd, direct_file_buffer_ + send_index_, direct_file_n_ - send_index_, 0);
		if (s <= 0) return false;
		send_index_ += s;
	}
	else
		send_state_ = SENT;
	return true;
}

ResponseState HttpResponse::sendResponsePart(int socket_fd)
{
	if (!res_ready_) return ERROR;

	// std::cout << "Send state " << respStateToText(send_state_) << std::endl;

	if (send_state_ == NOT_SENT)
	{
		send_state_ = HEADER;
		if (req_.getRequestError() == NO_HTTP_VERSION) send_state_ = BODY;

		send_index_ = 0;
	}

	if (send_state_ == HEADER)
	{
		size_t to_send = serialized_header_.size();

		if (send_index_ < to_send)
		{
			ssize_t sent = send(socket_fd, serialized_header_.c_str() + send_index_, to_send - send_index_, 0);

			if (sent <= 0) return ERROR;

			send_index_ += sent;
		}
		else
		{
			send_state_ = BODY;
			send_index_ = 0;
		}
	}

	if (send_state_ == BODY)
	{
		if (req_.getMethod() == GET)
		{
			if (file_status_ == FILE_OK)
			{
				size_t to_send = file_->data.size();

				if (send_index_ < to_send)
				{
					ssize_t sent = send(socket_fd, file_->data.data() + send_index_, to_send - send_index_, 0);
					if (sent <= 0) return ERROR;
					send_index_ += sent;
				}
				else send_state_ = SENT;
			}
			else if (file_status_ == FILE_STREAM_DIRECT)
			{
				if (!sendFileDirectPart(socket_fd)) return ERROR;
			}
			else
			{
				size_t to_send = body_.size();

				if (send_index_ < to_send)
				{
					ssize_t sent = send(socket_fd, body_.data() + send_index_, to_send - send_index_, 0);
					if (sent <= 0) return ERROR;
					send_index_ += sent;
				}
				else send_state_ = SENT;
			}
		}
		else
		{
			
		}
	}

	return send_state_;
}

void HttpResponse::clear()
{
	status_code_ = 0;
	status_message_.clear();

	headers_.clear();
	body_.clear();

	if (direct_file_fd_ >= 0) {
		close(direct_file_fd_);
		direct_file_fd_ = -1;
	}

	file_ = NULL;
	file_status_ = FILE_OK;
	file_path_.clear();

}
