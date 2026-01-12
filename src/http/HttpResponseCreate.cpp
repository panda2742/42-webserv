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
			handleExistingFile();
			return ;
		}
	}

	setHeader("Content-Type", "text/html");
	std::string error = to_string(code) + " " + status_message_;
	std::string body = "<html><head><title>"+error+"</title></head><body><center><h1>"+error+"</h1></center><hr><center>webserv/" VERSION "</center></body></html>";
	std::vector<char> body_vec(body.begin(), body.end());
	setBody(body_vec);
}

void HttpResponse::setDirectory()
{
	if (!req_.getLocation().getAutoindex()) return setError(403);

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

void HttpResponse::getRealRoot()
{
	root_ = req_.getLocation().getRoot();

	vecstr_t target = split(req_.getTarget(), '/');
	target.erase(target.begin(), target.begin() + req_.getLocation().getRoute().size());

	depth_in_loc_ = target.size();

	for (vecstr_t::iterator it = target.begin(); it != target.end(); it++)
		root_ += "/" + *it;
}

void HttpResponse::handleExistingFile()
{
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
}

void HttpResponse::createDefault()
{
	headers_["Content-Length"] = "0";

	const Location&	target = req_.getLocation();

	if ((target.getAllowMethods() & req_.getMethod()) == 0)
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

	// addCookie("test", "kakoukakou");
	// addCookie("test2", "kakoukakou2", true, true, 3600, "/", "Lax");

	cgi_t cgi = req_.getLocation().getCgi();

	std::string root_backup = root_;
	std::vector<std::string> path_info;
	int test_depth = 0;

	while (test_depth <= depth_in_loc_)
	{
		FileStatus status_test = FileCacheManager::testFile(root_);

		if (status_test == FILE_IS_DIR && test_depth == 0)
		{
			std::vector<std::string> indexes = req_.getLocation().getIndex();

			for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
			{
				std::string index_path = root_ + "/" + *it;

				FileStatus index_status = FileCacheManager::testFile(index_path);

				if (index_status == FILE_OK)
				{
					root_ = index_path;
					root_backup = index_path;
					status_test = FILE_OK;
				}
			}
		}

		if (status_test == FILE_OK)
		{
			if (!cgi.enabled) break;

			std::map<std::string, std::string>::iterator cgi_map = cgi.map.find(getExtension(root_));
			if (cgi_map != cgi.map.end())
			{
				for (std::vector<std::string>::reverse_iterator it = path_info.rbegin(); it != path_info.rend(); it++)
					path_info_ += *it;
				useCGI(cgi_map->second, root_);
				return ;
			}

			break;
		}
		else if (status_test == FILE_NOT_FOUND)
		{
			if (!cgi.enabled) break;

			size_t pos = root_.find_last_of('/');

			if (pos == std::string::npos) break;

			path_info.push_back(root_.substr(pos, root_.size()));
			root_.erase(pos, root_.size());
			std::cout << root_ << std::endl;
			test_depth++;
			continue;
		}
		else break;

	}
	root_ = root_backup;

	if (req_.getMethod() == METHOD_GET)
	{

		std::cout << "OEOEOEO" << std::endl;
		if (file_status_ == NONE) file_status_ = FileCacheManager::getFile(root_, file_, file_info_, file_path_);

		if (file_status_ == FILE_OK || file_status_ == FILE_STREAM_DIRECT) handleExistingFile();
		else if (file_status_ == FILE_IS_DIR) setDirectory();
		else if (file_status_ == PATH_FORBIDDEN) setError(400);
		else if (file_status_ == FILE_NOT_FOUND) setError(404);
		else if (file_status_ == FILE_FORBIDDEN) setError(403);
		else if (file_status_ == PATH_TO_LONG) setError(414);
		else setError(500);

		return;
	}
	else if (req_.getMethod() == METHOD_POST)
	{
		upload_t	upload = target.getUpload();
		if (upload.enabled)
		{
			std::deque<UploadExtractData>	extracted = extractUpload(req_.getBody(), req_.getContentSize());
			for (std::deque<UploadExtractData>::const_iterator it = extracted.begin(); it != extracted.end(); ++it)
			{
				if ((*it).error != 200)
				{
					setError((*it).error);
					return;
				}
			}
		}



	}

	// Si delete autorise + DELETE -> delete file

	setError(405);
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


std::deque<HttpResponse::UploadExtractData>	HttpResponse::extractUpload(char *body, size_t size) const
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

	std::deque<UploadExtractData>	extract_data;
	std::string						flag;
	size_t							flag_len = 0;
	size_t							i = 0;

	for (; i < size; ++i)
	{
		if (flag.length() == 0)
		{
			if (std::find(charset.begin(), charset.end(), body[i]) != charset.end())
				flag = std::string(body, body + flag_len);
			else
				++flag_len;
		}
		else
			break;
	}

	if (!flag.length())
	{
		extract_data.push_front(UploadExtractData(400));
		return extract_data;
	}

	while (i < size)
	{
		const std::string	file(&body[i], size - i);
		const size_t		next_flag_pos = file.find(flag);
		const size_t		filename_pos = file.find("filename=");
		if (next_flag_pos == std::string::npos || filename_pos == std::string::npos)
		{
			extract_data.push_front(UploadExtractData(400));
			return extract_data;
		}

		std::string			filename;
		const std::string	tmp_filename_cstr = file.substr(filename_pos + 9);
		const char			*filename_cstr = tmp_filename_cstr.c_str();
		size_t		j = 0;
		for (; j < size; ++j)
		{
			if (j == 0)
			{
				if (filename_cstr[0] != '"')
				{
					extract_data.push_front(UploadExtractData(400));
					return extract_data;
				}
				continue;
			}
			if (filename.length() == 0)
			{
				if (filename_cstr[j] == '"')
					filename = std::string(&filename_cstr[1], j - 1);
			}
			else break;
		}
		if (!filename.length())
		{
			extract_data.push_front(UploadExtractData(400));
			return extract_data;
		}

		std::string	content = file.substr(0, next_flag_pos);

		const size_t		end_header_pos = content.find("\r\n\r\n");
		if (end_header_pos == std::string::npos)
		{
			extract_data.push_front(UploadExtractData(400));
			return extract_data;
		}
		content = content.substr(end_header_pos + 4);

		const std::string	caught = file.substr(next_flag_pos, flag.length() + 2);
		const bool			is_end_flag = caught == std::string(flag + "--");

		// std::cout << "- Content=" << content << "\n- Caught=" << caught << "\n- Is end flag? " << is_end_flag
		// 	<< "\n- Filename=" << filename
		// 	<< "\n===============================================" << std::endl;
		UploadExtractData	extract = UploadExtractData(200);
		extract.body = content;
		extract.body_size = content.size();
		extract.filename = filename;
		extract_data.push_back(extract);

		if (is_end_flag)
			return extract_data;
		i += next_flag_pos + flag.length();
	}

	return extract_data;
}

