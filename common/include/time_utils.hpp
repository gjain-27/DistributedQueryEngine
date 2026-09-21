#pragma once

#include <ctime>

inline std::tm toUtcTm(std::time_t time) {
	std::tm result{};
#ifdef _WIN32
	gmtime_s(&result, &time);
#else
	gmtime_r(&time, &result);
#endif
	return result;
}