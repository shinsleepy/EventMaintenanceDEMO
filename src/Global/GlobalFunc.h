#ifndef _GLOBAL_FUNC_H_
#define _GLOBAL_FUNC_H_

#include <ctime> 
#include <string>
#include <sstream>
#include <iomanip>

#define ADJUST_TIME_TEST // only use to test

//DocTime is similar to tm. It's the early time form which planners assign in CSV
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

inline std::tm Localtime(time_t timestamp)
{
	std::tm Result{};
#ifdef _WIN32
	localtime_s(&Result, &timestamp);
#else
	localtime_r(&timestamp, &Result);
#endif
	return Result;
}

inline std::string TimeToString(time_t timestamp)
{
	auto TM_ = Localtime(timestamp);

	std::ostringstream oss;
	oss << std::put_time(&TM_, "%Y-%m-%d %H:%M");
	return oss.str();
}

inline time_t StringToTime(const std::string& timeStr)
{
	std::tm TM_ = {};
	std::istringstream iss(timeStr);

	iss >> std::get_time(&TM_, "%Y-%m-%d %H:%M");
	if (iss.fail())
		return 0;
	
	return mktime(&TM_);
}

#ifdef ADJUST_TIME_TEST
extern time_t DebugDeltaTime;

inline void SetCurrentTime(time_t CurrentTime)
{
	
	DebugDeltaTime = CurrentTime - time(NULL);
}
#endif //ADJUST_TIME_TEST

inline time_t GetCurrentTime()
{
#ifndef ADJUST_TIME_TEST
	return time(NULL);
#else //ADJUST_TIME_TEST
	return time(NULL) + DebugDeltaTime;
#endif //ADJUST_TIME_TEST
}

#endif //_GLOBAL_FUNC_H_
