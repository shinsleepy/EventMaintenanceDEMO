#ifndef _EVENT_H_
#define _EVENT_H_

#include <ctime>
#include "Tools/Table/TableManager.h"
#include "Tools/Log/LogManager.h"
#include <set>

enum eEventState
{
	EES_NONE,
	EES_EVE,
	//EES_SHOW,
	EES_ONGOING,
	EES_STOP,
	EES_FINISH,
};

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
		State = EES_NONE;
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
			return EES_FINISH;
		}
			 

		switch (State)
		{
		case EES_EVE:
			{
				if (CurrentTime < RoundTime[0])
					break;

				StateChange(EES_ONGOING);
				LogManager::Instance().Log(E_LOG_TYPE_INFO, "Event Started. EventID:%d", EventID);
			}
			break;
		case EES_ONGOING:
			{
				HandleSyncState();

				if (CurrentTime < RoundTime[1])
					break;

				if (pSetting_->DurationType == eEventDurationType::E_DURATION_PERMANENT)
					break;

				StateChange(EES_STOP);
				LogManager::Instance().Log(E_LOG_TYPE_INFO, "Event Stopped. EventID:%d", EventID);
			}
			break;
		case EES_STOP:
		{
			HandleSyncState();

			// TODO: check if sync complete

			ComputeNextRoundTime(CurrentTime);
			if (RoundTime[0] == 0) // no next round
			{
				StateChange(EES_FINISH);
			}
			else
			{
				StateChange(EES_EVE);
			}

		}
			break;
		case EES_FINISH: //will be destroyed in Update()
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
		//case eEventDurationType::E_DURATION_WEEK_DAYS:
		//{
		//	std::set<int> WeekDaySet_;
		//	auto WeekDayNum_ = pSetting_->DurationValue;
		//	auto TempWeekDay_ = 0;

		//	while (WeekDayNum_ > 0)
		//	{
		//		TempWeekDay_ = int(WeekDayNum_ % 10);
		//		WeekDayNum_ /= 10;

		//		if (TempWeekDay_ <= 7)
		//		{
		//			WeekDaySet_.insert(TempWeekDay_);
		//		}
		//	}

		//	// should fix to thread safe version
		//	auto CurrentTM_ = *localtime(&CurrentTime);
		//	tm NextTM_ = {};

		//	if (CurrentTime < EventTime[0])
		//	{ 
		//		NextTM_ = *localtime(&EventTime[0]); 
		//	}
		//	else
		//	{
		//		NextTM_ = CurrentTM_;
		//		NextTM_.tm_hour = localtime(&pSetting_->StartTime)->tm_hour;
		//	}

		//	auto StartWeekDay_ = NextTM_.tm_wday;
		//}
		//	break;
		//case eEventDurationType::E_DURATION_REPEAT:
		//	break;
		default:
			break;
		}

				
	}

	void StateChange(eEventState NewState)
	{
		LogManager::Instance().Log(E_LOG_TYPE_INFO, "Event State Change. EventID:%d From %d To %d", EventID, State, NewState);

		State = NewState;

		HandleSyncState();
	}

	void HandleSyncState()
	{
		// TODO: notify other server
	}
};

#endif //_EVENT_H_

