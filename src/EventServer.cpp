#include "EventServer.h"
#include "Tools/Table/TableManager.h"
#include "Tools/Log/LogManager.h"
#include <set>
#include <iostream>
#include "Global/GlobalFunc.h"
#include <string>

EventServer& EventServer::Instance()
{
    static EventServer s_Instance;
    return s_Instance;
}

int EventServer::Init()
{
	ReadLastEvent();
	ReadCSV();
	ShowType();
	return E_RETURN_CODE_OK;
}

int EventServer::Reload()
{
	ReadCSV();
	return E_RETURN_CODE_OK;
}

void EventServer::ReadLastEvent()
{
	// TODO: recovery events from DB or ini 
}

void EventServer::RecordCurrentEvent()
{
	// TODO: record current events to DB or ini
}

void EventServer::ReadCSV()
{
	auto CurrentTime_ = GetCurrentTime();

	auto EventSettings_ = TableManager::Instance().GetAllEventSetting();

	for (auto Iter_ = EventSettings_.begin(); Iter_ != EventSettings_.end(); ++Iter_)
	{
		if (Iter_->second.StartTime > Iter_->second.EndTime)
		{
			LogManager::Instance().Log(E_LOG_TYPE_ERROR, "Event CSV Error. Event:%d StartTime > EndTime", Iter_->first);
			continue;
		}

		auto EventIter_ = _EventMap.find(Iter_->first);
		if (EventIter_ == _EventMap.end())
		{
			if (CurrentTime_ > Iter_->second.EndTime && Iter_->second.DurationType != eEventDurationType::E_DURATION_PERMANENT)
				continue;

			// Add new event
			EventUnit NewEvent_;
			EventIter_ = _EventMap.insert_or_assign(Iter_->first, NewEvent_).first; 
			//EventIter_ = _EventMap.emplace(Iter_->first, NewEvent_).first;
			NewEvent_.Init();
		}

		EventIter_->second.EventID = Iter_->first;
		EventIter_->second.EventTime[0] = Iter_->second.StartTime;
		EventIter_->second.EventTime[1] = Iter_->second.EndTime;

		EventIter_->second.ComputeNextRoundTime(CurrentTime_);

		if (CurrentTime_ >= EventIter_->second.RoundTime[0])
		{
			EventIter_->second.State = eEventState::EES_ONGOING;
			LogManager::Instance().Log(E_LOG_TYPE_INFO, "Add New Event Just After ReadCSV. EventID:%d", EventIter_->first);
		}
		else
			EventIter_->second.State = eEventState::EES_EVE;

		// TODO: prepare sync flag to notify other servers
	}

	// check event lost setting
	// a way to close event
	for (auto Iter_ = _EventMap.begin(); Iter_ != _EventMap.end(); ++Iter_)
	{
		if (Iter_->second.GetSetting() == nullptr)
			Iter_->second.State = eEventState::EES_STOP;
	}
}

void EventServer::Show(int EventID)
{
	auto Iter_ = _EventMap.find(EventID);
	if (Iter_ == _EventMap.end())
		return;

	auto pSetting_ = Iter_->second.GetSetting();
	if (!pSetting_)
		return;

	std::cout << "EventID:" << Iter_->first << " EventType:" << EventTypeToString(pSetting_->EventType) <<" State:" << EventStateToString(Iter_->second.State) << " From:" << TimeToString(Iter_->second.RoundTime[0]) << " To:"<< TimeToString(Iter_->second.RoundTime[1]) << std::endl;
}

void EventServer::ShowType(eEventType Type)
{	
	for (auto Iter_ = _EventMap.begin(); Iter_ != _EventMap.end(); ++Iter_)
	{
		auto pSetting_ = Iter_->second.GetSetting();
		if (!pSetting_)
			continue;

		if (Type == eEventType::E_EVENT_TYPE_NONE) // show all event
			Show(Iter_->first);
		else if (pSetting_->EventType == Type)
			Show(Iter_->first);
	}
}

EventUnit* EventServer::GetEvent(int EventID)
{
	auto Iter_ = _EventMap.find(EventID);
	if (Iter_ == _EventMap.end())
		return nullptr;
	return &Iter_->second;
}

void EventServer::Update(time_t CurrentTime)
{
	auto Iter_ = _EventMap.begin();

	while(Iter_ != _EventMap.end())	
	{
		if (Iter_->second.CheckState(CurrentTime) == eEventState::EES_FINISH)
		{
			LogManager::Instance().Log(E_LOG_TYPE_INFO, "Remove finished event. EventID:%d", Iter_->first);
			Iter_ = _EventMap.erase(Iter_);
		}	
		else
			++Iter_;
	}
}

void EventServer::Delete(int EventID)
{
	auto pEvent_ = GetEvent(EventID);
	if (!pEvent_)
	{
		LogManager::Instance().Log(E_LOG_TYPE_WARNING, "Event not exist to delete. EventID:%d", EventID);
		return;
	}

	pEvent_->State = eEventState::EES_FINISH;
	LogManager::Instance().Log(E_LOG_TYPE_WARNING, "Delete event manually. EventID:%d", EventID);
}
