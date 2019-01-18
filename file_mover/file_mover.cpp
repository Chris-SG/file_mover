// file_mover.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "directory_copy.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	std::string copypath;
	std::vector<destination> destination_vec;
	std::ifstream config;
	config.open("config.ini");
	std::string line;
	while (std::getline(config, line))
	{
		if (line.substr(0, 1) == "#"){}
		else if (line.find("COPYPATH=") != std::string::npos)
		{
			copypath = line.substr(line.find_first_of('"')+1, std::string::npos);
			copypath = copypath.substr(0, copypath.find_last_of('"'));
		}
		else if (line.find("DEST=") != std::string::npos)
		{
			auto dest = line.substr(line.find_first_of('=') + 1, std::string::npos);
			destination dest_toadd;
			dest_toadd.share = dest.substr(0, dest.find_first_of(','));
			dest.erase(0, dest_toadd.share.length()+1);
			dest_toadd.user = dest.substr(0, dest.find_first_of(','));
			dest.erase(0, dest_toadd.user.length()+1);
			dest_toadd.password = dest.substr(0, dest.find_first_of(','));
			dest.erase(0, dest_toadd.password.length()+1);
			dest_toadd.path = dest.substr(0, std::string::npos);

			destination_vec.push_back(dest_toadd);
		}
	}
	config.close();
	if (copypath.empty())
		return 2;
	logger::open("logs/" + logger::time_now() + ".txt");
	directory_copy copy(std::filesystem::path(copypath.c_str()));
	std::vector<std::thread> threads;
	for (auto& a : destination_vec)
	{
		threads.push_back(std::thread(&directory_copy::copy, copy, a.share, a.user, a.password, a.path));
		//auto ret = copy.copy(a.share, a.user, a.password, a.path);
	}

	for (auto& a : threads)
	{
		if(a.joinable())
			a.join();
	}

	fs::recursive_directory_iterator end;
	for (fs::recursive_directory_iterator it(copypath.c_str()); it != end;)
	{
		auto fpath = *it++;
		if (fs::is_regular_file(fpath))
		{
			fs::remove(fpath);
		}
	}

	logger::close();
	std::cout << "All done! Press any key to exit...";
	std::cin.get();
	return 0;
}
