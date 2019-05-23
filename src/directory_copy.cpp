#include <filesystem>
#include <iostream>
#include <sstream>


#include "directory_copy.hpp"

#pragma comment(lib,"Mpr.lib") 

namespace fs = std::filesystem;

std::mutex directory_copy::m = std::mutex();

directory_copy::directory_copy(fs::path inpath)
{
	a_inpath = inpath;
}

directory_copy::~directory_copy()
{
}

bool directory_copy::copy(std::string share, std::string user, std::string password, std::string to_path)
{
	auto open_code = open_connection(const_cast<LPSTR>(share.c_str()), const_cast<LPSTR>(user.c_str()), const_cast<LPSTR>(password.c_str()));
	if (open_code == ERROR_SESSION_CREDENTIAL_CONFLICT)
	{
		m.lock();
		std::cout << "Connection pre-established for " << share << ".\n";
		m.unlock();
	}
	else if (open_code != NO_ERROR)
	{
		LPSTR buf = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, open_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, nullptr);
		m.lock();
		std::stringstream err;
		err << "Failed to open connection " << share << ". Error code: " << open_code << " " << buf << "\n";
		std::cerr << err.str();
		logger::write(err.str());
		m.unlock();
		close_connection(const_cast<LPSTR>(share.c_str()));

		return false;
	}
	else
	{
		m.lock();
		std::cout << "Connection established for " << share << ".\n";
		m.unlock();
	}

	try
	{
		m.lock();
		std::cout << "Copying files from " << a_inpath << ". Please wait...\n";
		m.unlock();
		std::string buf;
		auto create_dir = [&a_inpath = a_inpath](fs::directory_entry a, fs::path to) { CreateDirectoryA(to.string().c_str(), 0); };
		auto create_file = [&a_inpath = a_inpath](fs::directory_entry a, fs::path to) { CopyFileA(a.path().string().c_str(), to.string().c_str(), 0); };
		auto create_other = [&a_inpath = a_inpath](fs::directory_entry a, fs::path to) { CopyFileA(a.path().string().c_str(), to.string().c_str(), 1); };
		for (auto& a : fs::recursive_directory_iterator(a_inpath))
		{
			buf = a.path().string();
			if (buf.find(a_inpath.string()) != std::string::npos)
			{
				m.lock();
				std::cout << "Copying " << a.path().filename() << " to " << share << ".\n";
				m.unlock();
				buf.erase(0, a_inpath.string().length());
				fs::path to(share);
				to /= fs::path(to_path + buf);
				try
				{
					if (fs::is_directory(a) && !fs::exists(to)) create_dir(a, to);
					else if (fs::is_regular_file(a)) create_file(a, to);
					else create_other(a, to);
				}
				catch (std::exception& e)
				{
					m.lock();
					std::stringstream err;

					err << "Failed file " << to << " : " << e.what() << "\n";
					std::cerr << err.str();
					logger::write(err.str());
					m.unlock();
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::stringstream err;
		err << "Error:" << e.what() << "\n";
		std::cerr << err.str();
		logger::write(err.str());
	}
	m.lock();
	std::cout << "Done with " << share << ".\n";
	m.unlock();
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