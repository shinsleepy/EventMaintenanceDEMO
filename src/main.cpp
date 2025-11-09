#include <iostream>
#include "Common/Log/LogManager.h"
#include "Common/Table/TableManager.h"
#include "Common/Global/GlobalConst.h"
#include <thread>
#include <atomic>

using namespace std;

std::atomic<bool> g_Running = true;

int InitEventServer()
{
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

	//Server
	if (InitEventServer())
	{
		return E_RETURN_CODE_FAILED;
	}

	return E_RETURN_CODE_OK;
}

void MainProcess()
{
	time_t CurrentTime_ = 0;
	while (g_Running)
	{
		// run once per sec
		auto temp_ = time(NULL);
		if (temp_ != CurrentTime_)
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
	std::string input;
	while (g_Running) {
		std::getline(std::cin, input);
		if (input == "exit") {
			std::cout << "[Server] Exit command received.\n";
			g_Running = false;
		}
		else if (!input.empty()) {
			std::cout << "[Server] Unknown command: " << input << std::endl;
		}
	}
}

int main()
{
	if (Init())
	{
		LogManager::Instance().Log(E_LOG_TYPE_ERROR, "Init Failed");
		return E_RETURN_CODE_FAILED;
	}

	std::thread cmdThread(CommandListener);

	MainProcess();
	
	g_Running = false;
	if (cmdThread.joinable())
		cmdThread.join();

	Close();

	return E_RETURN_CODE_OK;
}





