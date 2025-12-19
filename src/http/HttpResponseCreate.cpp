#include "http/HttpResponse.hpp"
#include "FileCacheManager.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <dirent.h>

void HttpResponse::setStatus(int code, const std::string &message)
{
	status_code_ = code;
	status_message_ = message;
}

void HttpResponse::setHeader(const std::string &name, const std::string &value)
{
	headers_[name] = value;
}

void HttpResponse::setBody(const std::vector<char> &body) {
	body_ = body;
	headers_["Content-Length"] = to_string(body.size());
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

void HttpResponse::setError(int code)
{
	setStatus(code, getHttpErrorMessage(code));

	// const std::map<unsigned int, std::string>& error_pages = req_.getServerInstance()->getErrorPages();
	// std::map<unsigned int, std::string>::const_iterator error = error_pages.find(static_cast<unsigned int>(code));
	// if (error != error_pages.end()) 
	// {
	// 	CachedFile *file_error = NULL;
	// 	struct stat file_error_info = {};
	// 	std::string final_file_path;
	// 	// TODO modif pour pas prendre le root mais passer par les locations
	// 	FileStatus err_file_status = FileCacheManager::getFile(req_.getServerInstance()->getRoot(), error->second, file_error, file_error_info, final_file_path);
		
	// 	if (err_file_status)
	// }
	
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

	FileStatus index_status = FileCacheManager::getFile(req_.getServerInstance()->getRoot(), index_path, file_, tmp_file_info, full_path_tmp);


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

void HttpResponse::createDefault()
{
	headers_["Content-Length"] = "0";

	// addCookie("test", "kakoukakou");
	// addCookie("test2", "kakoukakou2", true, true, 3600, "/", "Lax");

	// useCGI("/usr/bin/php-cgi", "/home/lilefebv/Documents/cursus/42-webserv/www/script.php");
	// return ;

	// if (req_.getTarget() == "/abc")
	// {
	// 	setRedirect(302, "/OEOEOEOE");
	// 	return ;
	// } // Redirection example
	if (req_.getMethod() == GET)
	{
		if (file_status_ == NONE) file_status_ = FileCacheManager::getFile(req_.getServerInstance()->getRoot(), req_.getTarget(), file_, file_info_, file_path_);

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
	case HTTPS_REQUEST:
		setError(501); break;
	case BAD_REQUEST:
	default:
		setError(400); break;
	}

	if (res_ready_) return ;
	if (!waiting_cgi_) serializeHeader();

	res_ready_ = true;
}
