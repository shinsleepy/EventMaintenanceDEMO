#ifndef _EVENT_SERVER_H_
#define _EVENT_SERVER_H_

#include "Global/GlobalConst.h"
#include <map>
#include "Event.h"

class EventServer
{
private:
	std::map<int, EventUnit> _EventMap;

public:
	static EventServer& Instance();
	int Init();
	int Reload();

	void ReadLastEvent();
	void RecordCurrentEvent();
	void ReadCSV();
	void Show(int EventID); 
	void ShowType(eEventType Type = eEventType::E_EVENT_TYPE_NONE);

	void Update(time_t CurrentTime); // run in MainProcess

private:
	EventServer() = default;
};

#endif //_EVENT_SERVER_H_
