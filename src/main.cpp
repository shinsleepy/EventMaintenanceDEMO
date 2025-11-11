#include <iostream>
#include "Tools/Log/LogManager.h"
#include "Tools/Table/TableManager.h"
#include "Tools/Command/CommandManager.h"
#include "Global/GlobalConst.h"
#include <thread>
#include <atomic>
#include "EventServer.h"
#include "Global/GlobalFunc.h"

using namespace std;

std::atomic<bool> g_Running = true;

int InitCommand()
{
	auto& CommandManager_ = CommandManager::Instance();

	CommandManager_.RegisterCommand("exit", [](const std::vector<std::string>&)
		{
			std::cout << "Exiting server...\n";
			g_Running = false;
		}, 
		"Exit the server");

	CommandManager_.RegisterCommand("help", [](const std::vector<std::string>&)
		{
			std::cout << "Show commands...\n";
			CommandManager::Instance().ShowHelp();
		},
		"Show all commands and their desc");

	CommandManager_.RegisterCommand("show_event", [](const std::vector<std::string>&)
		{
			std::cout << "Show events...\n";
			EventServer::Instance().ShowType();
		},
		"Show events");

	CommandManager_.RegisterCommand("reload", [](const std::vector<std::string>&)
		{
			std::cout << "Reload CSV...\n";
			TableManager::Instance().Reload();
			EventServer::Instance().Reload();
		},
		"Reload CSV");

#ifdef ADJUST_TIME_TEST
	CommandManager_.RegisterCommand("set_server_time", [](const std::vector<std::string>& args)
		{
			std::cout << "Set Server Time...\n";
			if (args.empty()) 
			{
				DebugDeltaTime = 0;
			}
			else
			{
				SetCurrentTime(StringToTime(args[0]+ " " + args[1]));
			}

			LogManager::Instance().Log(E_LOG_TYPE_WARNING, "CurrentTime Set To:%s", TimeToString(GetCurrentTime()).c_str());
			
		},
		"Set Server Time (For Test Only). Parameter Should be YY-MM-DD HH:MM");

	CommandManager_.RegisterCommand("get_server_time", [](const std::vector<std::string>& args)
		{
			std::cout << "get Server Time...\n";
			LogManager::Instance().Log(E_LOG_TYPE_INFO, "CurrentTime Is:%s", TimeToString(GetCurrentTime()).c_str());
		},
		"Get Server Time (For Test Only)");
#endif //ADJUST_TIME_TEST

	return E_RETURN_CODE_OK;
}

int Init()
{
	if (LogManager::Instance().Start())
		return E_RETURN_CODE_FAILED;
	LogManager::Instance().Log(E_LOG_TYPE_INFO, "LogManager Init Complete");

	if (TableManager::Instance().Init("./table/")) // assign table path
		return E_RETURN_CODE_FAILED;
	LogManager::Instance().Log(E_LOG_TYPE_INFO, "TableManager Init Complete");

	// Server
	if (EventServer::Instance().Init())
	{
		return E_RETURN_CODE_FAILED;
	}
	LogManager::Instance().Log(E_LOG_TYPE_INFO, "EventServer Init Complete");

	// Init Command
	if (InitCommand())
		return E_RETURN_CODE_FAILED;

	return E_RETURN_CODE_OK;
}

void MainProcess()
{
	time_t CurrentTime_ = 0;
	while (g_Running)
	{	
		auto temp_ = GetCurrentTime();
		if (temp_ != CurrentTime_)// run once per sec
		{
			CurrentTime_ = temp_;
			//LogManager::Instance().Log(E_LOG_TYPE_INFO, "Heartbeat %d", CurrentTime_);

			EventServer::Instance().Update(CurrentTime_);
		}
	}
}

int Close()
{
	LogManager::Instance().Stop();

	return E_RETURN_CODE_OK;
}

void CommandListener()
{
	std::string Input_;
	while (g_Running) {
		std::getline(std::cin, Input_);
		if (Input_.empty())
			continue;
		CommandManager::Instance().Execute(Input_);
	}
}

int main()
{
	if (Init())
	{
		LogManager::Instance().Log(E_LOG_TYPE_ERROR, "Init Failed");
		return E_RETURN_CODE_FAILED;
	}

	std::thread CmdThread_(CommandListener);

	MainProcess();
	
	g_Running = false;
	if (CmdThread_.joinable())
		CmdThread_.join();

	Close();

	return E_RETURN_CODE_OK;
}





