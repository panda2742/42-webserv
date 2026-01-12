#ifndef __FILECACHEMANAGER_HPP__
# define __FILECACHEMANAGER_HPP__

#include <map>
#include <vector>
#include <string>
#include <time.h>
#include <ctime>
#include <sys/stat.h>
#include <queue>

#define MAX_CACHE_FILE_SIZE 10000 // 4194304
#define MAX_CACHE_SIZE 1073741824 // 2^30

std::string getExtension(const std::string &path);

enum FileStatus
{
	NONE,
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
	time_t last_open;
};

struct Entry
{
	std::string path;
	time_t last_open;

	bool operator>(Entry const& other) const { return last_open > other.last_open; }
};

class FileCacheManager
{

private:
	
	static size_t cache_size_;
	static std::map<std::string, CachedFile> cache_;
	static std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry> > lru_heap_;

	static void freeMinimumSize(size_t required);
	static void touch(const std::string& path);

public:
	FileCacheManager();
	~FileCacheManager();

	static FileStatus getFile(const std::string& path, CachedFile*& file, struct stat &fileInfo, std::string &full_path);
	static FileStatus testFile(const std::string& path);

};



#endif
