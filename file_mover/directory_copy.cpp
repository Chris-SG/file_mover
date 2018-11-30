#include "pch.h"
#include "directory_copy.hpp"

namespace fs = std::filesystem;

directory_copy::directory_copy(fs::path inpath)
{
	a_inpath = inpath;
}

directory_copy::~directory_copy()
{
	fs::recursive_directory_iterator end;
	for (fs::recursive_directory_iterator it(a_inpath); it != end;)
	{
		auto fpath = *it++;
		if (fs::is_regular_file(fpath))
		{
			fs::remove(fpath);
		}
	}
}

bool directory_copy::copy(std::string share, std::string user, std::string password, std::string to_path)
{
	auto open_code = open_connection(const_cast<LPSTR>(share.c_str()), const_cast<LPSTR>(user.c_str()), const_cast<LPSTR>(password.c_str()));
	if (open_code == ERROR_SESSION_CREDENTIAL_CONFLICT)
		std::cout << "Connection pre-established for " << share << "." << std::endl;
	else if (open_code != NO_ERROR)
	{
		LPSTR buf = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, open_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, nullptr);
		std::cout << "Failed to open connection " << share << ". Error code: " << open_code << " " << buf <<  std::endl;
		close_connection(const_cast<LPSTR>(share.c_str()));

		return false;
	}
	else
		std::cout << "Connection established for " << share << "." << std::endl;

	try
	{
		std::cout << "Copying files. Please wait..." << std::endl;
		fs::copy(a_inpath, fs::path(share + to_path), fs::copy_options::update_existing | fs::copy_options::recursive);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	close_connection(const_cast<LPSTR>(share.c_str()));
	return true;
}

DWORD directory_copy::open_connection(LPSTR path, LPSTR user, LPSTR password)
{
	NETRESOURCEA resource;
	resource.dwType = RESOURCETYPE_DISK;
	resource.lpLocalName = 0;
	resource.lpRemoteName = path;
	resource.lpProvider = 0;
	return WNetAddConnection2A(&resource, password, user, CONNECT_TEMPORARY);
}

DWORD directory_copy::close_connection(LPSTR path)
{
	return WNetCancelConnection2A(path, 0, 1);
}
