#ifndef _TABLE_MANAGER_H_
#define _TABLE_MANAGER_H_

#include <string>
#include "EventTable.h"
#include <map>

/*
This file determine the behaviors of an TableManager
TableManager maintain all game setting in CSVs
TableManager should read the CSVs when the server setup
Arrange them to suitable forms for any query requests from other system in this server
Depends on the needs, some CSVs should allow to be reloaded
*/

class TableManager
{
private:
    std::string _TablePath = "";

    std::map<int, EventTableEntry> _EventTableSetting; // <EventID, EventSetting>

public:
    static TableManager& Instance();
    ~TableManager();

    int Init(std::string Path);
    void Clear();
    void SetPath(std::string Path);
    int Reload();

    const std::map<int, EventTableEntry>& GetAllEventSetting() const;
    const EventTableEntry* GetEventEntry(int EventID);

private:
    TableManager();

    int ReadEventTable();
};

#endif //_TABLE_MANAGER_H_
