#include "CommandManager.h"
#include "../Log/LogManager.h"

CommandManager& CommandManager::Instance()
{
    static CommandManager s_Instance;
    return s_Instance;
}

// Register the command name, func, and desc
void CommandManager::RegisterCommand(const std::string& Name, const std::function<void(const std::vector<std::string>&)>& Func, const std::string& Desc)
{
    _Commands[Name] = Func;
    _Descriptions[Name] = Desc;
}

// Fetch the input string from user, execute if it's a registered command
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
        LogManager::Instance().Log(E_LOG_TYPE_COUT, "Unknown command: %s\n", Cmd_.c_str());
    }
}

// list all the registered commands
void CommandManager::ShowHelp()
{
    LogManager::Instance().Log(E_LOG_TYPE_COUT, "Available commands:\n");
    for(auto Iter_ = _Descriptions.begin(); Iter_ != _Descriptions.end(); ++Iter_) 
    {
        if (!Iter_->second.empty())
            LogManager::Instance().Log(E_LOG_TYPE_COUT, " - %s : %s\n", Iter_->first.c_str(), Iter_->second.c_str());
        else
            LogManager::Instance().Log(E_LOG_TYPE_COUT, " - %s\n", Iter_->first.c_str(), Iter_->second.c_str());
    }
}

// Spread a input string into tokens
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
