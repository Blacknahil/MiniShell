#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

#include "helper.h"


const char* ENV_VAR_NAME = "PATH";

bool isEscape(QUOTE_STATE state, char c)
{
  if (state == QUOTE_STATE::IN_DOUBLE_QUOTE)
  {
    if (DOUBLE_QUOTES_ESCAPES.find(c) != DOUBLE_QUOTES_ESCAPES.end())
    {
      return true;
    }
      // std::cout << "false can not be found: " << c << "\n";
    return false;
  }
  else 
  {
    // std::cout << "state unknown " << c << "\n";
    return true;
  }
}

void concatenateString(std::string& output, std::vector<std::string>& arr, int index)
{
  for (size_t i= index; i<arr.size()-1; i++)
  {
    output+= arr[i];
    output +=" ";
  }
  output += arr[arr.size()-1];
}

bool checkPath(std::string& output, const std::string& query)
{
  const char* path_value = getenv(ENV_VAR_NAME);
  if (path_value == nullptr)
  {
    std::cerr << ENV_VAR_NAME << " not found!\n";
    return false;
  }

  std::string pathStr(path_value);

  std::stringstream pathss(pathStr);
  std::string singlePathDir;

  while(std::getline(pathss, singlePathDir, ':'))
  {
    // check if the singlePathDir + query is a full path dir 
    std::filesystem::path fullPath = std::filesystem::path(singlePathDir) / query;
    if(!std::filesystem::exists(fullPath))
    {
     continue;
    }
    if (access(fullPath.c_str(), X_OK) == 0)
    {
      output = fullPath.string();
      return true;
    }
    // and if it is an excutable 
  }
  return false;
}

bool checkBuiltin(std::string command)
{
  const std::vector<std::string> builtins = {"type", "echo", "exit","pwd", "cd"};
  for(const std::string& builtin:builtins)
  {
    if (command == builtin)
    {
      return true;
    }
  }
  return false;
}
void excuteProgram(std::vector<std::string>& parsed_args)
{
  pid_t pid = fork();
  if (pid == -1)
  {
    std::cerr << "fork failed" << std::endl;
    return;
  }
  else if (pid == 0)
  {
    std::vector<char*> cStyleArgs;
    for (size_t i=0; i < parsed_args.size(); ++i)
    {
      // std::cout << "arg: " << parsed_args[i] << std::endl;
      cStyleArgs.push_back(strdup(parsed_args[i].c_str()));
    }
    cStyleArgs.push_back(nullptr);
    execvp(parsed_args[0].c_str(), cStyleArgs.data());
  }
  else 
  {
    int status;
    waitpid(pid, &status, 0);
  }
}

void pwd(std::string& output)
{
  std::filesystem::path current_path = std::filesystem::current_path();
  output = current_path.string();

}

bool changeDirectory(const std::string& pathStr)
{
  // check if tilde
  if (pathStr == "~")
  {
    const char* homePathvalue = getenv("HOME");
    if (homePathvalue == nullptr)
    {
    std::cerr << ENV_VAR_NAME << " not found!\n";
    return false;
    }
    chdir(homePathvalue);
    return true;
  }

  std::filesystem::path path = std::filesystem::path(pathStr);
  if (!std::filesystem::exists(path) ||
      !std::filesystem::is_directory(path))
  {
    return false;
  }

  if (chdir(pathStr.c_str()) == 0)
  {
    return true;
  }

  return false;

}