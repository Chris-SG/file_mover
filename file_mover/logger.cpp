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
		return true;
	}
	return false;
}



std::string logger::format_time(std::chrono::hours hr, std::string fmt)
{
	static uint32_t epoch_year = 1970;
	uint32_t years = 0;
	uint32_t hours = hr.count() % 24;
	uint32_t days = ((hr.count() - hours) / 24) + 1;
	uint32_t months = 1;
	bool complete = false;
	while (!complete)
	{
		if (is_leap_year(epoch_year + years))
		{
			if (days > 366)
			{
				++years;
				days -= 366;
			}
			else
				complete = true;
		}
		else
		{
			if (days > 365)
			{
				++years;
				days -= 365;
			}
			else
				complete = true;
		}
	}

	std::initializer_list<uint8_t> months_30({ 4, 6, 9, 11 });
	std::initializer_list<uint8_t> months_31({ 1, 3, 5, 7, 8, 10, 12 });

	while (days > 28)
	{
		if (std::find(std::begin(months_30), std::end(months_30), months))
		{
			if (days <= 30)
				break;
			else
			{
				days -= 30;
				++months;
			}
		}
		else if (std::find(std::begin(months_31), std::end(months_31), months))
		{
			if (days <= 31)
				break;
			else
			{
				days -= 31;
				++months;
			}
		}
		else
		{
			if (is_leap_year(years + epoch_year) && days > 29)
			{
				days -= 29;
				++months;
			}
			else if (days > 28)
			{
				days -= 29;
				++months;
			}
		}
	}

	fmt.replace(fmt.find("%Y"), 2, std::to_string(epoch_year + years));
	fmt.replace(fmt.find("%m"), 2, std::to_string(months));
	fmt.replace(fmt.find("%d"), 2, std::to_string(days));

	return fmt;
}

bool logger::is_leap_year(uint32_t year)
{
	if (year % 4 != 0)
		return false;
	else if (year % 100 != 0)
		return true;
	else if (year % 400 != 0)
		return false;
	return true;
}