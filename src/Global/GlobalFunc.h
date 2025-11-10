#ifndef _GLOBAL_FUNC_H_
#define _GLOBAL_FUNC_H_

#include <ctime> 
#include <string>
#include <sstream>
#include <iomanip>

struct DocTime
{
	int Year;
	int Month;
	int Date;
	int Hour;
	DocTime()
	{
		memset(this, 0, sizeof(*this));
	}

	time_t ToTimeT()
	{
		struct tm t = {};
		t.tm_year = Year - 1900;
		t.tm_mon = Month - 1;
		t.tm_mday = Date;
		t.tm_hour = Hour;
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_isdst = -1;

		return mktime(&t);
	}
};

inline std::string TimeToString(time_t timestamp)
{
	auto TM_ = *localtime(&timestamp);

	std::ostringstream oss;
	oss << std::put_time(&TM_, "%Y-%m-%d %H:%M");
	return oss.str();
}

#endif //_GLOBAL_FUNC_H_
