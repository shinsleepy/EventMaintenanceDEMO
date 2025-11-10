#include <iostream>
#include "Tools/Log/LogManager.h"
#include "Tools/Table/TableManager.h"
#include "Tools/Command/CommandManager.h"
#include "Global/GlobalConst.h"
#include <thread>
#include <atomic>

using namespace std;

std::atomic<bool> g_Running = true;

int InitEventServer()
{
	return E_RETURN_CODE_OK;
}

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
	if (InitEventServer())
	{
		return E_RETURN_CODE_FAILED;
	}

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
		auto temp_ = time(NULL);
		if (temp_ != CurrentTime_)// run once per sec
		{
			CurrentTime_ = temp_;

			LogManager::Instance().Log(E_LOG_TYPE_INFO, "Heartbeat %d", CurrentTime_);
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





