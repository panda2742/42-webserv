#include "FileCacheManager.hpp"
#include "Logger.hpp"
#include "utils.hpp"
#include <sys/stat.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <string.h>
#include <errno.h>

size_t FileCacheManager::cache_size_ = 0;
std::map<std::string, CachedFile> FileCacheManager::cache_;

FileCacheManager::FileCacheManager()
{

}

FileCacheManager::~FileCacheManager()
{

}

void debug_log_file(struct stat fileInfo)
{
	std::cout << "Type:         : ";
	if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) {
		std::cout << "Directory\n";
	} else {
		std::cout << "File\n";
	}

	std::cout << "Size          : " <<
		fileInfo.st_size << '\n';               // Size in bytes
	std::cout << "Device        : " <<
		(char)(fileInfo.st_dev + 'A') << '\n';  // Device number
	std::cout << "Created       : " <<
		std::ctime(&fileInfo.st_ctim.tv_sec);         // Creation time
	std::cout << "Modified      : " <<
		std::ctime(&fileInfo.st_mtim.tv_sec);         // Last mod time
}

std::string getExtension(const std::string &path)
{
	std::string::size_type slashPos = path.find_last_of("/\\");
	if (slashPos == std::string::npos)
		slashPos = 0;
	else
		slashPos += 1;

	std::string::size_type dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos || dotPos < slashPos) return "";

	return path.substr(dotPos);
}

bool readFileToVector(const std::string& path, std::vector<char>& out)
{
	std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
	
	if (!file.is_open()) return false;

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0)
		return false;

	out.resize(size);

	if (!file.read(&out[0], size))
		return false;

	return true;
}

FileStatus FileCacheManager::getFile(std::string path, CachedFile*& file, struct stat &fileInfo, std::string &full_path)
{
	file = NULL;
	std::string basepath = "./www/";
	full_path = basepath + path;

	if (path.find("..") != std::string::npos) return PATH_FORBIDDEN;

	if (stat(full_path.c_str(), &fileInfo) != 0)
	{
		if (errno == ENOENT) return FILE_NOT_FOUND;
		else if (errno == EACCES) return FILE_FORBIDDEN;
		else if (errno == EBADF) return INTERNAL_ERROR;
		else if (errno == EFAULT) return INTERNAL_ERROR;
		else if (errno == ELOOP) return INTERNAL_ERROR;
		else if (errno == ENAMETOOLONG) return PATH_TO_LONG;
		else if (errno == ENOMEM) return INTERNAL_ERROR;
		else if (errno == ENOTDIR) return FILE_NOT_FOUND;
		else return(INTERNAL_ERROR);
	}

	if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) return FILE_IS_DIR;
	if (fileInfo.st_size > MAX_CACHE_FILE_SIZE) return FILE_STREAM_DIRECT;
	
	// debug_log_file(fileInfo);

	std::map<std::string, CachedFile>::iterator it = cache_.find(path);
	if (it == cache_.end())
	{
		cache_.insert(std::make_pair(path, CachedFile()));

		it = cache_.find(path);

		CachedFile& file_tmp = it->second;

		file_tmp.mtime = fileInfo.st_mtime;
		file_tmp.mime = getMimeType(getExtension(path));
		
		if (!readFileToVector(full_path, file_tmp.data))
		{
			cache_.erase(it);
			return INTERNAL_ERROR;
		}

		file_tmp.size = file_tmp.data.size();

		Logger::info("file " + path + " cached");
	}

	file = &it->second;

	return FILE_OK;
}
