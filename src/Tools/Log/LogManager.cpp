#include "LogManager.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <iostream>
#include "../../Global/GlobalFunc.h"

LogManager& LogManager::Instance() 
{
    static LogManager s_Instance;
    return s_Instance;
}

LogManager::LogManager() : m_Running(false)
{}

LogManager::~LogManager()
{
    Stop();
}

int LogManager::Start()
{
    m_Running = true;
    m_Thread = std::thread(&LogManager::WorkerThread, this);
    
    return 0;
}

void LogManager::Stop()
{
    m_Running = false;
    m_CondVar.notify_all();
    if (m_Thread.joinable())
        m_Thread.join();
}

std::string LogManager::GetCurrentTimeString() 
{
    std::time_t now = std::time(nullptr);
    std::tm localTime = Localtime(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &localTime);
    return buf;
}

void LogManager::Log(eLogType type, const char* format, ...) 
{
    char msgBuf[1024];
    va_list args;
    va_start(args, format);
    std::vsnprintf(msgBuf, sizeof(msgBuf), format, args);
    va_end(args);

    LogMessage msg;
    msg.type = type;
    msg.text = msgBuf;
    msg.timestamp = GetCurrentTimeString();

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Queue.push(std::move(msg));
    }
    m_CondVar.notify_one();
}

// Main loop, run by a new thread
void LogManager::WorkerThread() 
{
    while (m_Running || !m_Queue.empty()) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_CondVar.wait(lock, [&]() {
            return !m_Running || !m_Queue.empty();
            });

        while (!m_Queue.empty()) {
            LogMessage msg = std::move(m_Queue.front());
            m_Queue.pop();
            lock.unlock();

            const char* colorCode = "";
            const char* typeStr = "";
            switch (msg.type) {
            case eLogType::E_LOG_TYPE_INFO:
                colorCode = "\033[0;37m"; typeStr = "INFO"; break;
            case eLogType::E_LOG_TYPE_WARNING:
                colorCode = "\033[1;33m"; typeStr = "WARNING"; break;
            case eLogType::E_LOG_TYPE_ERROR:
                colorCode = "\033[1;31m"; typeStr = "ERROR"; break;
            }

            if(msg.type == E_LOG_TYPE_COUT)
                std::printf("%s", msg.text.c_str());
            else
                std::printf("%s[%s] [%s] %s\033[0m\n", colorCode, msg.timestamp.c_str(), typeStr, msg.text.c_str());

            // TODO: append to file
            
            lock.lock();
        }
    }
}


