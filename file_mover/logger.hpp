#pragma once
#include "pch.h"


namespace logger
{
	static std::ofstream logfile = std::ofstream();

	std::string time_now();

	bool open(std::string log);

	bool close();

	bool write(std::string write);
}