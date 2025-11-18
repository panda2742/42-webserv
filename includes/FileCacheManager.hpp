#ifndef __FILECACHEMANAGER_HPP__
# define __FILECACHEMANAGER_HPP__

#include <map>
#include <vector>
#include <string>
#include <time.h>
#include <ctime>
#include <sys/stat.h>

#define MAX_CACHE_FILE_SIZE 4194304
#define MAX_CACHE_SIZE 1073741824 // 2^30

extern const std::map<std::string, std::string> MIME_TABLE;

std::string getExtension(const std::string &path);

enum FileStatus
{
	FILE_OK,
	FILE_STREAM_DIRECT,
	FILE_IS_DIR,
	FILE_NOT_FOUND,
	FILE_FORBIDDEN,
	PATH_FORBIDDEN,
	PATH_TO_LONG,
	INTERNAL_ERROR
};

struct CachedFile
{
    std::vector<char> data;
    time_t mtime; // last change on disk to avoid sending old version
    std::string mime;
	size_t size;
};

class FileCacheManager
{

private:
	
	static size_t cache_size_;
	static std::map<std::string, CachedFile> cache_;

public:
	FileCacheManager();
	~FileCacheManager();

	static FileStatus getFile(std::string path, CachedFile*& file, struct stat &fileInfo);

};



#endif
