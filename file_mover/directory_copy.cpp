#include "pch.h"
#include "directory_copy.hpp"

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
					if (fs::is_regular_file(a))
					{
						CopyFileA(a.path().string().c_str(), to.string().c_str(), 0);
						//fs::copy(a, to, fs::copy_options::update_existing);
					}
					else
					{
						CopyFileA(a.path().string().c_str(), to.string().c_str(), 1);
						//fs::copy(a, to);
					}
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
		//fs::copy(a_inpath, fs::path(share + to_path), fs::copy_options::update_existing | fs::copy_options::recursive);
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