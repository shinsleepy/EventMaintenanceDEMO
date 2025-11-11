
# EventMaintenanceDemo

**EventMaintenanceDemo** is a demonstration project showcasing the framework of an **EventServer**.
It does not include any network or database functionality, but focuses solely on how to manage in-game events for online games.
The demo provides several methods to handle situations that a real EventServer might encounter — including adding or removing events while the server is running, modifying event schedules, and offering tools to test and verify event timing behavior.


## Features

### EventServer
- Maintain events
- Update each second to check state change
- Refresh by **reload** command

### TableManager
- Read **EventTable.csv** in **table** folder
- Keep EventTable entries as event setting
- Refresh by **reload** command

### LogManager
- Show log on screen, not write any LogFile currently
- A new thread to serve this

### CommandManager
- Listen to user's command
- Use **help** to list all commands
- A new thread to serve this
- Register commands in main.cpp for different kinds of servers
## Installation

1. **Install Visual Studio 2022 (or later)**
2. These settings may helpful
- Windows SDK Version: 10.0
- Platform Toolset: Visual Studio 2022 (v143)
- C++ Language Standard: ISO C++14 Standard (Default)
- C Language Standard: MSVC Standard (Default)
3. **Build** the project
4. Beware that when you run the demo. There should be a folder called **table** in your Working folder, otherwise you can change the path in main.cpp

``` 
int Init()
{
	...

	if (TableManager::Instance().Init("./table/")) // assign table path
		return E_RETURN_CODE_FAILED;	

    ...
}
``` 
    
## Usage/Examples

### Check Commands
- Execute **help** command
<img width="750" height="181" alt="image" src="https://github.com/user-attachments/assets/a03030af-fca4-4931-94b3-5569d4d8abac" />

### Add Event
- Add a new entry to EventTable.csv. Make sure that time settings is suitable
- Execute **reload** command

### Delete Event
1. Delete by CSV(recommended)
- Remove the entry in EventTable.csv
- Execute **reload** command


2. Delete by command(not recommended)
- Execute **delete EventID** command
<img width="855" height="223" alt="image" src="https://github.com/user-attachments/assets/89f8468c-5d8a-488e-971a-a991f6a45727" />

### Event Time Test
- There is a define ADJUST_TIME_TEST in ./src/Global/GlobalFunc.h
```
#define ADJUST_TIME_TEST // only use to test
```

- If the define exist. There are two commands usable

    1. The **set_server_time** command can assign your input(should be YYYY-MM-DD HH:MM) as server time. Then you can check if events run as your prediction. 
    2. The **get_server_time** can show when is your server time now.
    3. If your want to reset the server time back to the machine(computer) time. Execute **set_server_time** without any parameters
<img width="638" height="256" alt="image" src="https://github.com/user-attachments/assets/ff25b141-dd7c-4f7e-9801-db0827e39418" />


## TODO

### Functions related to time
Time functions(time(), localtime()...etc) in this demo should change to a thread save version. And try to keep it still cross-platform.

### Record event
Record current events before server closed. And recovery recorded events after server reboot.

### Sync flags
If network functionality is implemented, this server would be able to communicate with other servers. There should be sync flags or similar mechanisms to keep the event states consistent. For example, you could implement an EventClient on other servers. These EventClients would register with the EventServer, allowing the EventServer to know which clients should be — and have already been — notified of event state changes.
## Authors

- [@shinsleepy](https://www.github.com/shinsleepy)







