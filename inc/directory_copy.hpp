#pragma once

#include <mutex>
#include <Windows.h>

#include "logger.hpp"

struct destination
{
	std::string share;
	std::string user;
	std::string password;
	std::string path;
};

class directory_copy
{
private:
	static std::mutex m;
	std::filesystem::path a_inpath;


	DWORD open_connection(LPSTR path, LPSTR user, LPSTR password);
	DWORD close_connection(LPSTR path);

public:
	directory_copy(std::filesystem::path inpath);
	~directory_copy();
	bool copy(std::string share, std::string user, std::string password, std::string to_path);
};