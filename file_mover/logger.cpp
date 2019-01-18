#include "pch.h"
#include "logger.hpp"

std::string logger::time_now()
{

	/*auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	tm time;
	localtime_s(&time, &in_time_t);
	ss << std::put_time(time, "%Y-%m-%d %X");
	return ss.str();*/

	std::time_t t = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &t);
	std::stringstream ss;
	ss << std::put_time(&tm, "%Y-%m-%d %X");
	return ss.str();
}

bool logger::open(std::string log)
{
	std::replace(log.begin(), log.end(), ':', '-');
	logfile.open(log);
	std::cout << "logger " << log << " " << logfile.is_open() << "\n";
	return logfile.is_open();
}

bool logger::close()
{
	if (logfile.is_open())
	{
		logfile.close();
		return true;
	}
	return false;
}

bool logger::write(std::string write)
{
	if (logfile.is_open())
	{
		logfile << "[" << time_now() << "]" << write << "\n";
	}
}