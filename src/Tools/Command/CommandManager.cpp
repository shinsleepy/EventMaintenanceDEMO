#include "CommandManager.h"

CommandManager& CommandManager::Instance()
{
    static CommandManager s_Instance;
    return s_Instance;
}

void CommandManager::RegisterCommand(const std::string& Name, const std::function<void(const std::vector<std::string>&)>& Func, const std::string& Desc)
{
    _Commands[Name] = Func;
    _Descriptions[Name] = Desc;
}

//void CommandManager::EnqueueCommand(const std::string& Cmd)
//{
//    std::lock_guard<std::mutex> lock(_Mutex);
//    _Queue.push(Cmd);
//}
//
//bool CommandManager::TryDequeueCommand(std::string& OutCmd)
//{
//    std::lock_guard<std::mutex> lock(_Mutex);
//    if (_Queue.empty()) return false;
//    OutCmd = std::move(_Queue.front());
//    _Queue.pop();
//    return true;
//}
//
//void CommandManager::ExecutePending()
//{
//    std::string Cmd_;
//    while (TryDequeueCommand(Cmd_)) {
//        Execute(Cmd_);
//    }
//}

void CommandManager::Execute(const std::string& Input)
{
    auto Tokens_ = Tokenize(Input);
    if (Tokens_.empty()) return;

    const std::string& Cmd_ = Tokens_[0];
    auto Key_ = _Commands.find(Cmd_);
    if (Key_ != _Commands.end()) {
        std::vector<std::string> args(Tokens_.begin() + 1, Tokens_.end());
        Key_->second(args);
    }
    else {
        std::cout << "Unknown command: " << Cmd_ << "\n";
    }
}

void CommandManager::ShowHelp()
{
    std::cout << "Available commands:\n";
    for(auto Iter_ = _Descriptions.begin(); Iter_ != _Descriptions.end(); ++Iter_) 
    {
        std::cout << " - " << Iter_->first;
        if (!Iter_->second.empty())
            std::cout << " : " << Iter_->second;
        std::cout << "\n";
    }
}

std::vector<std::string> CommandManager::Tokenize(const std::string& Input)
{
    std::istringstream iss(Input);
    std::vector<std::string> Tokens_;
    std::string Word_;
    while (iss >> Word_) {
        Tokens_.push_back(Word_);
    }
    return Tokens_;
}
