#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

#include "helper.h"

namespace fs = std::filesystem;

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

void concatenateString(std::string& output, std::vector<std::string>& arr, size_t start, size_t end)
{
  for (size_t i= start; i < end-1; i++)
  {
    output+= arr[i];
    output +=" ";
  }
  output += arr[end-1];
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
    fs::path fullPath = fs::path(singlePathDir) / query;
    if(!fs::exists(fullPath))
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
void excuteProgram(std::vector<std::string>& parsed_args, size_t end)
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
    for (size_t i=0; i < end; ++i)
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
  fs::path current_path = fs::current_path();
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

  fs::path path = fs::path(pathStr);
  if (!fs::exists(path) ||
      !fs::is_directory(path))
  {
    return false;
  }

  if (chdir(pathStr.c_str()) == 0)
  {
    return true;
  }

  return false;

}

RedirectOutput checkRedirection(std::vector<std::string>& argList)
{
  size_t length = argList.size();
  for(size_t i=0; i < length; i++)
  {
    if (argList[i] == ">" || argList[i] == "1>")
    {
      return RedirectOutput{true,i};
    }
  }
  return RedirectOutput{false,0};
}

void writeToFile(std::string& content, std::string& fileName)
{
  fs::path path(fileName);

  if (path.has_parent_path())
  {
    fs::create_directories(path.parent_path());
  }

  std::ofstream ofs(path);
  if (!ofs)
  {
    std::cerr << "failed to open file " << path;
    return;
  }
  ofs << content;

}

int excuteAndCapture(std::string& output,
                    size_t end,
                     std::vector<std::string>& args)
{
  int fds[2];
  pid_t pid;

  if (pipe(fds) == -1)
  {
    perror("fork");
    return -1;
  }
  pid = fork();

  if (pid == pid_t (0))
  {
    // this is the child process where i want to run the command and channel back the output of the command 
    // close read end : parent process 
    close(fds[0]);

    dup2(fds[1], STDOUT_FILENO); // redirect stdout of the child process to pipe end 
    dup2(fds[1], STDERR_FILENO);// redirect the stderr as well to the pipe end as well
    close(fds[1]);

    std::vector<char*> cStyleArgs;
    for (size_t i = 0; i < end; i++)
    {
      cStyleArgs.push_back(strdup(args[i].c_str()));
    }
    cStyleArgs.push_back(nullptr);
    execvp(args[0].c_str(), cStyleArgs.data());
    perror("execvp failed");
    _exit(1);
  }

  // this is the parent process 
  // close the write end of the child process 
  char buffer[4096];
  ssize_t n;
  while ((n = read(fds[0], buffer, sizeof(buffer))) > 0)
  {
    output.append(buffer, n);
  }

  close(fds[0]);
  // wait for the child process to finish 
  waitpid(pid, NULL, 0);
  return 0;
}