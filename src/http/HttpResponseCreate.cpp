#include "http/HttpResponse.hpp"
#include "FileCacheManager.hpp"
#include "program.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <dirent.h>
#include <algorithm>

void HttpResponse::setStatus(int code, const std::string &message)
{
	if (!status_mutable_) return ;
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

	const std::string *error_path = req_.getLocation().getErrorPage(code);

	if (error_path)
	{
		CachedFile *file_error = NULL;
		struct stat file_error_info = {};
		std::string final_file_path;
		// TODO modif pour pas prendre le root mais passer par les locations
		FileStatus err_file_status = FileCacheManager::getFile(*error_path, file_error, file_error_info, final_file_path);

		if (err_file_status == FILE_OK || err_file_status == FILE_STREAM_DIRECT)
		{
			file_path_ = final_file_path;
			file_info_ = file_error_info;
			file_status_ = err_file_status;
			file_ = file_error;
			status_mutable_ = false;
			createDefault();
			return ;
		}
	}

	// try load configurated error page
	// + set file status a FILE_OK si y'a bien un fichier / FILE_STREAM_DIRECT si trop lourd

	setHeader("Content-Type", "text/html");
	std::string error = to_string(code) + " " + status_message_;
	std::string body = "<html><head><title>"+error+"</title></head><body><center><h1>"+error+"</h1></center><hr><center>webserv/" VERSION "</center></body></html>";
	std::vector<char> body_vec(body.begin(), body.end());
	setBody(body_vec);
}

void HttpResponse::setDirectory()
{
	std::vector<std::string> indexes = req_.getLocation().getIndex();

	std::string baseTarget = root_[root_.size() - 1] == '/' ? root_ : root_ + "/";

	for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
	{
		struct stat tmp_file_info;
		std::string full_path_tmp;
		std::string index_path = baseTarget + *it;
	
		FileStatus index_status = FileCacheManager::getFile(index_path, file_, tmp_file_info, full_path_tmp);
	
		if (index_status == FILE_OK || index_status == FILE_STREAM_DIRECT)
		{
			file_path_ = full_path_tmp;
			file_info_ = tmp_file_info;
			file_status_ = index_status;

			if (testUseCGI(index_path)) return ;

			createDefault();
			return ;
		}
	}

	if (!req_.getLocation().getAutoindex()) return setError(403);

	std::string dirname = (root_[root_.size() - 1] == '/') ? root_ : (root_ + "/");
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

void HttpResponse::getRealRoot()
{
	root_ = req_.getLocation().getRoot();

	vecstr_t target = split(req_.getTarget(), '/');
	target.erase(target.begin(), target.begin() + req_.getLocation().getRoute().size());

	for (vecstr_t::iterator it = target.begin(); it != target.end(); it++)
		root_ += "/" + *it;
}

bool HttpResponse::testUseCGI(const std::string& cgi_prog)
{
	cgi_t cgi = req_.getLocation().getCgi();
	if (cgi.enabled)
	{
		std::map<std::string, std::string>::iterator cgi_map = cgi.map.find(getExtension(cgi_prog));
		if (cgi_map != cgi.map.end())
		{
			file_status_ = NONE;
			useCGI(cgi_map->second, cgi_prog);
			return true;
		}
	}
	return false;
}

void HttpResponse::createDefault()
{
	headers_["Content-Length"] = "0";


	// VERIFY LOCATION

	// std::vector<std::string> res = split(req_.getTarget(), '/');

	// std::cout << cfg::util::represent(res) << std::endl;

	const Location&	target = req_.getLocation();

	if ((target.getAllowMethods() & req_.getMethod()) == 0 && status_mutable_)
	{
		setError(405);
		return;
	}

	redirect_t	redir = target.getRedirection();

	if (redir.enabled)
	{
		setRedirect(redir.code, redir.route);
		return;
	}

	upload_t	upload = target.getUpload();
	// if (upload.enabled)
	// {
	std::cout << "UPLOAD BODY: " << req_.getBody() << std::endl;
	extractUpload(req_.getBody(), req_.getContentSize());
	// }

	// addCookie("test", "kakoukakou");
	// addCookie("test2", "kakoukakou2", true, true, 3600, "/", "Lax");

	// useCGI("/usr/bin/php-cgi", "/home/lilefebv/Documents/cursus/42-webserv/www/script.php");
	// return ;

	if (req_.getMethod() == METHOD_GET || !status_mutable_)
	{
		if (file_status_ == NONE) file_status_ = FileCacheManager::getFile(root_, file_, file_info_, file_path_);

		if (file_status_ == FILE_OK)
		{
			if (testUseCGI(root_)) return ;
			
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
		if (testUseCGI(root_)) return ; // Ajouter le test pour les index
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
		getRealRoot();
		createDefault();
		break;
	case NOT_HTTP_HEADER:
		return ;
	case UNKNOWN_METHOD:
		setError(405); break;
	case UNSUPPORTED_HTTP_VERSION:
		setError(505); break;
	case HTTPS_REQUEST:
		setError(501); break;
	case BODY_TOO_LONG:
		setError(413); break;
	case BAD_REQUEST:
	default:
		setError(400); break;
	}

	if (res_ready_) return ;
	if (!waiting_cgi_) serializeHeader();

	res_ready_ = true;
}


std::vector<
HttpResponse::UploadExtractData>	HttpResponse::extractUpload(char *body, size_t size) const
{
	static std::vector<char>	charset;
	if (charset.empty())
	{
		charset.push_back(' ');
		charset.push_back('\n');
		charset.push_back('\t');
		charset.push_back('\r');
		charset.push_back('\v');
	}

	std::vector<UploadExtractData>	extract_data;
	std::string						flag;
	size_t							flag_len;

	for (size_t i = 0; i < size; ++i)
	{
		if (flag.length() == 0)
		{
			std::cout << '<' << (int)body[i] << "> " << std::cout;
			if (std::find(charset.begin(), charset.end(), body[i]) != charset.end())
				flag = std::string(body, body + flag_len);
			else
				++flag_len;
		}
	}

	std::cout << "Flag is " << flag << std::endl;

	return extract_data;
}

