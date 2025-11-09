#ifndef _EVENT_H_
#define _EVENT_H_

enum eEventState
{
	EES_NONE,
	EES_EVE,
	EES_SHOW,
	EES_ONGOING,
	EES_STOP,
	EES_FINISH,
};

struct EventUnit
{
	int ID;
};

#endif //_EVENT_H_

