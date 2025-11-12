#ifndef _TABLE_STRUCT_H_
#define _TABLE_STRUCT_H_

#include <ctime>
#include <string>

/*
This file describe the form of EventTable.csv
*/

// Event Type
#define EVENT_TYPE_LIST \
X(E_EVENT_TYPE_NONE) /* */\
X(E_EVENT_TYPE_TRIGGER) /* open spec state during event period */\
X(E_EVENT_TYPE_COMPETITION) /* start and finish rounds by customize round time */\

enum class eEventType
{
#define X(name) name,
	EVENT_TYPE_LIST
#undef X
};

inline const char* EventTypeToString(eEventType type) {
	switch (type) {
#define X(name) case eEventType::name: return #name;
		EVENT_TYPE_LIST
#undef X
	default: return "UNKNOWN";
	}
}

inline eEventType EventTypeFromString(const std::string& str) {
#define X(name) if (str == #name) return eEventType::name;
	EVENT_TYPE_LIST
#undef X
		return eEventType::E_EVENT_TYPE_NONE; // default
}

// Duration Type
#define EVENT_DURATION_TYPE_LIST \
X(E_DURATION_PERMANENT) /* always exist */\
X(E_DURATION_SINGLE_ROUND) /* round time = event time */\
X(E_DURATION_REPEAT) /* repeat depend on round time */\

enum class eEventDurationType
{
#define X(name) name,
	EVENT_DURATION_TYPE_LIST
#undef X
};

inline const char* EventDurationTypeToString(eEventDurationType type) {
	switch (type) {
#define X(name) case eEventDurationType::name: return #name;
		EVENT_DURATION_TYPE_LIST
#undef X
	default: return "UNKNOWN";
	}
}

inline eEventDurationType EventDurationTypeFromString(const std::string& str) {
#define X(name) if (str == #name) return eEventDurationType::name;
	EVENT_DURATION_TYPE_LIST
#undef X
		return eEventDurationType::E_DURATION_PERMANENT; // default
}

const int CUSTOM_VALUE_NUM = 5;

struct EventTableEntry
{
	int ID; // EventID
	eEventType EventType; // need to combine with CustomValue[] to implement different effect. Currently no effect
	eEventDurationType DurationType;
	int DurationValue; // additional value for duration type
	int RoundTimeMin; // minutes per round
	time_t StartTime;
	time_t EndTime;
	int CustomValue[CUSTOM_VALUE_NUM];
	EventTableEntry()
	{
		memset(this, 0, sizeof(*this));
	}
};

#endif //_TABLE_STRUCT_H_
