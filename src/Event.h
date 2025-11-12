#ifndef _EVENT_H_
#define _EVENT_H_

#include <ctime>
#include "Tools/Table/TableManager.h"
#include "Tools/Log/LogManager.h"
#include <set>

#define EVENT_STATE_LIST \
X(EES_NONE) /* */\
X(EES_EVE) /*  */\
X(EES_ONGOING) /*  */\
X(EES_STOP) /*  */\
X(EES_FINISH) /*  */\

enum class eEventState
{
#define X(name) name,
	EVENT_STATE_LIST
#undef X
};

inline const char* EventStateToString(eEventState type) {
	switch (type) {
#define X(name) case eEventState::name: return #name;
		EVENT_STATE_LIST
#undef X
	default: return "UNKNOWN";
	}
}

struct EventUnit
{	
	int EventID;
	eEventState State;
	
	time_t EventTime[2]; // [0]=StartTime [1]=EndTime
	time_t RoundTime[2]; // [0]=StartTime [1]=EndTime

	EventUnit()
	{
		memset(this, 0, sizeof(*this));
	}

	void Init()
	{
		State = eEventState::EES_NONE;
	}

	const EventTableEntry* GetSetting()
	{
		return TableManager::Instance().GetEventEntry(EventID);
	}

	eEventState CheckState(time_t CurrentTime)
	{
		auto pSetting_ = GetSetting();
		if (!pSetting_)
		{
			LogManager::Instance().Log(E_LOG_TYPE_ERROR, "Event Lost Setting When CheckState. EventID:%d", EventID);
			return eEventState::EES_FINISH;
		}
			 

		switch (State)
		{
		case eEventState::EES_EVE:
			{
				if (CurrentTime < RoundTime[0])
					break;

				StateChange(eEventState::EES_ONGOING);
				LogManager::Instance().Log(E_LOG_TYPE_INFO, "Event Started. EventID:%d", EventID);
			}
			break;
		case eEventState::EES_ONGOING:
			{
				HandleSyncState();

				if (CurrentTime < RoundTime[1])
					break;

				if (pSetting_->DurationType == eEventDurationType::E_DURATION_PERMANENT)
					break;

				StateChange(eEventState::EES_STOP);
				LogManager::Instance().Log(E_LOG_TYPE_INFO, "Event Stopped. EventID:%d", EventID);
			}
			break;
		case eEventState::EES_STOP:
		{
			HandleSyncState();

			// TODO: check if sync complete

			ComputeNextRoundTime(CurrentTime);
			if (RoundTime[0] == 0) // no next round
			{
				StateChange(eEventState::EES_FINISH);
			}
			else
			{
				StateChange(eEventState::EES_EVE);
			}

		}
			break;
		case eEventState::EES_FINISH: //will be destroyed in Update()
			break;
		default:
			break;
		}

		return State;
	}

	void ComputeNextRoundTime(time_t CurrentTime)
	{
		RoundTime[0] = 0;
		RoundTime[1] = 0;
		
		auto pSetting_ = GetSetting();
		if (!pSetting_)
			return;

		switch (pSetting_->DurationType)
		{
		case eEventDurationType::E_DURATION_PERMANENT:
			break;
		case eEventDurationType::E_DURATION_SINGLE_ROUND:
			{
				if (CurrentTime > EventTime[1])
					return;

				RoundTime[0] = EventTime[0];
				RoundTime[1] = EventTime[1];
			}
			break;
		case eEventDurationType::E_DURATION_REPEAT:
		{	
			time_t NextTime_ = 0;
			auto RoundSeconds_ = pSetting_->RoundTimeMin * 60;
			//const int ONE_MINUTE_SECOND = 60;

			if (CurrentTime < EventTime[0])
			{
				RoundTime[0] = EventTime[0];
				RoundTime[1] = RoundTime[0] + RoundSeconds_;
				break;
			}
			else
			{
				time_t Elapsed_ = CurrentTime - EventTime[0];
				int RoundIndex_ = static_cast<int>(Elapsed_ / RoundSeconds_);
				RoundTime[0] = EventTime[0] + RoundIndex_ * RoundSeconds_;
				RoundTime[1] = RoundTime[0] + RoundSeconds_;

				if (RoundTime[1] > EventTime[1])
					RoundTime[1] = EventTime[1];
			}
		}
			break;
		default:
			break;
		}

				
	}

	void StateChange(eEventState NewState)
	{
		LogManager::Instance().Log(E_LOG_TYPE_INFO, "Event State Change. EventID:%d From %s To %s", EventID, EventStateToString(State), EventStateToString(NewState));

		State = NewState;

		HandleSyncState();
	}

	void HandleSyncState()
	{
		// TODO: notify other server
	}
};

#endif //_EVENT_H_

