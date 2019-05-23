#pragma once

#include <fstream>
#include <chrono>
#include <mutex>

namespace logger
{
	static std::ofstream logfile = std::ofstream();
	static std::mutex m = std::mutex();

	std::string time_now();
	bool open(std::string log);
	bool close();
	bool write(std::string write);

	std::string format_time(std::chrono::milliseconds ms, std::string fmt);
	std::string format_time(std::chrono::seconds sec, std::string fmt);
	std::string format_time(std::chrono::minutes min, std::string fmt);
	std::string format_time(std::chrono::hours hr, std::string fmt);
	bool is_leap_year(uint32_t year);
}