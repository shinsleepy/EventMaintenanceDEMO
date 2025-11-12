#ifndef _COMMAND_MANAGER_H_
#define _COMMAND_MANAGER_H_

#include <unordered_map>
#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <mutex>

/*
This file determine the behaviors of an CommandManager
CommandManager wounld listen input from user, and fetch it
If it is a registered command, then execute it
*/
class CommandManager
{
private:
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> _Commands;
    std::unordered_map<std::string, std::string> _Descriptions;

public:
    static CommandManager& Instance();
    void RegisterCommand(const std::string& Name, const std::function<void(const std::vector<std::string>&)>& Func, const std::string& Desc = "");
    void Execute(const std::string& Input);
    void ShowHelp();

private:
    CommandManager() = default;
    
    std::vector<std::string> Tokenize(const std::string& Input);
};

#endif //_COMMAND_MANAGER_H_
