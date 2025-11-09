#include "TableManager.h"
#include "../Log/LogManager.h"
#include <fstream>
#include <sstream>
#include "../Global/GlobalFunc.h"

TableManager& TableManager::Instance() {
    static TableManager instance;
    return instance;
}

int TableManager::Init(std::string Path)
{
	SetPath(Path);
	
    if (Reload())
        return 1;

	return 0;
}

void TableManager::Clear()
{
    _EventTableSetting.clear();
}

void TableManager::SetPath(std::string Path)
{
	_TablePath = Path;
}

int TableManager::Reload()
{
    Clear();

    if (ReadEventTable())
        return 1;

    return 0;
}

TableManager::TableManager() 
{
}

TableManager::~TableManager()
{
}

int TableManager::ReadEventTable()
{
	EventTableEntry TempEntry_;
    DocTime DocTime_;
    int i = 0;
	std::string FullPath_ = _TablePath + "EventTable.csv";
	std::ifstream File_(FullPath_);

	if (!File_.is_open())
	{
		LogManager::Instance().Log(E_LOG_TYPE_ERROR, "Open File Failed. Path:%s", FullPath_);
		return 1;
	}

    std::string Line_;
    bool IsHeader_ = true;

    while (std::getline(File_, Line_)) {
        if (Line_.empty())
            continue;

        if (IsHeader_) {
            IsHeader_ = false;
            continue;
        }

        std::stringstream ss(Line_);
        std::string token;
        
        std::getline(ss, token, ',');
        TempEntry_.ID = std::stoi(token);

        std::getline(ss, token, ',');
        TempEntry_.EventType = EventTypeFromString(token);

        std::getline(ss, token, ',');
        TempEntry_.DurationType = EventDurationTypeFromString(token);

        std::getline(ss, token, ',');
        TempEntry_.DurationValue = std::stoi(token);

        std::getline(ss, token, ',');
        TempEntry_.RoundTimeMin = std::stoi(token);

        // Start Time
        std::getline(ss, token, ',');
        DocTime_.Year = std::stoi(token);

        std::getline(ss, token, ',');
        DocTime_.Month = std::stoi(token);

        std::getline(ss, token, ',');
        DocTime_.Date = std::stoi(token);

        std::getline(ss, token, ',');
        DocTime_.Hour = std::stoi(token);

        TempEntry_.StartTime = DocTime_.ToTimeT();

        // End Time
        std::getline(ss, token, ',');
        DocTime_.Year = std::stoi(token);

        std::getline(ss, token, ',');
        DocTime_.Month = std::stoi(token);

        std::getline(ss, token, ',');
        DocTime_.Date = std::stoi(token);

        std::getline(ss, token, ',');
        DocTime_.Hour = std::stoi(token);

        TempEntry_.EndTime = DocTime_.ToTimeT();

        // Custom Value
        for (i = 0; i < CUSTOM_VALUE_NUM; ++i)
        {
            std::getline(ss, token, ',');
            TempEntry_.CustomValue[i] = std::stoi(token);
        }

        _EventTableSetting[TempEntry_.ID] = TempEntry_;
    }

	return 0;
}
