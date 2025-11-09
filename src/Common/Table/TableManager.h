#ifndef _TABLE_MANAGER_H_
#define _TABLE_MANAGER_H_

#include <string>
#include "EventTable.h"
#include <map>

class TableManager
{
private:
    std::string _TablePath = "";

    std::map<int, EventTableEntry> _EventTableSetting; // <EventID, EventSetting>
public:
    static TableManager& Instance();

    int Init(std::string Path);
    void Clear();
    void SetPath(std::string Path);
    int Reload();

private:
    TableManager();
    ~TableManager();

    int ReadEventTable();


};

#endif //_TABLE_MANAGER_H_
