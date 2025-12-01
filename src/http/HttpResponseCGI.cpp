#include "HttpResponse.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <algorithm>

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
		close(pipe_in[0]);
		close(pipe_out[1]);

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
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		throw std::runtime_error("CGI execve failed");
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
