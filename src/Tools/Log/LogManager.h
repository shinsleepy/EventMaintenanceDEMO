#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

/*
This file determine the behaviors of a LogManager
LogManager is responsible for showing(or writing to file if needed) logs
*/

enum eLogType 
{
    E_LOG_TYPE_COUT, // purely output string, not include other info(time, type...etc)
    E_LOG_TYPE_INFO,
    E_LOG_TYPE_WARNING,
    E_LOG_TYPE_ERROR,
};

struct LogMessage 
{
    eLogType type;
    std::string text;
    std::string timestamp;
};

class LogManager 
{
public:
    static LogManager& Instance();
    ~LogManager();

    int Start();
    void Stop();

    void Log(eLogType type, const char* format, ...);

private:
    LogManager();

    void WorkerThread();
    std::string GetCurrentTimeString();

private:
    std::thread m_Thread;
    std::queue<LogMessage> m_Queue;
    std::mutex m_Mutex;
    std::condition_variable m_CondVar;
    std::atomic<bool> m_Running;
};

#endif //_LOG_MANAGER_H_
