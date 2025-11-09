#ifndef _GLOBAL_FUNC_H_
#define _GLOBAL_FUNC_H_

#include <ctime> 
#include <string>

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

#endif //_GLOBAL_FUNC_H_
