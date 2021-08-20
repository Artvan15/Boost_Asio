#pragma once

#include <string>
#include <ctime>

std::string GetCurrentDate()
{
	std::time_t now = time(nullptr); //get current time
	char str[126] = {};
	ctime_s(str, sizeof(str), &now); //convert into readable format
	return str;
}