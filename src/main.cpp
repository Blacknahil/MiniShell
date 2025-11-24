#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <unordered_set>


enum QUOTE_STATE {NORMAL, IN_SINGLE_QUOTE, IN_DOUBLE_QUOTE};
const char* ENV_VAR_NAME = "PATH";
const char SINGLE_QUOTE = '\'';
const char DOUBLE_QUOTE = '\"';
const char BACKSLASH = '\\';
const std::unordered_set<char> DOUBLE_QUOTES_ESCAPES {'\\', '\"'};

bool changeDirectory(const std::string& pathStr);
bool checkBuiltin(std::string command);
bool checkPath(std::string& output, const std::string& query);
void concatenateString(std::string& output, std::vector<std::string>& arr, int index);
void excuteProgram(std::vector<std::string>& parsed_args);
bool isEscape(QUOTE_STATE state, char c);
void pwd(std::string& output);
void tokenizer(std::vector<std::string>& argList, char delimter, std::string& input, int& argc);
void prepareArgsExternal(std::string& output, std::vector<std::string>& arr, int index);


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true)
  {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> argv;
    int argc = 0;
    tokenizer(argv, ' ', input,argc);
    // std::cout << "size : " << argc <<  "argv[0]"<< argv[0];
    if (argv[0] == "exit")
    {
      if (argc >1 && argv[1] == "1")
      {
        return 1;
      }
      return 0;
    }
    else if (argc >=2 && argv[0]=="echo")
    {
      std::string output;
      // std::cout << "length : " << argv.size() -1 << "\n";
      concatenateString(output, argv, 1);
      std::cout << output <<"\n";
    }

    else if (argc >= 2 && argv[0] == "type")
    {
      if (checkBuiltin(argv[1]))
      {
        std::cout << argv[1] << " is a shell builtin\n";
      }
      else 
      {
        std::string concatenated;
        concatenateString(concatenated, argv, 1);
        std::string pathOutput;

        if (checkPath(pathOutput,argv[1]))
        {
          std::cout << concatenated << " is " << pathOutput << "\n";

        }
        else 
        {
           std::cout << concatenated <<": not found\n";
        }
       
      }
    }
    else if (argc ==1 && argv[0] =="pwd")
    {

      std::string output;
      pwd(output);
      std::cout << output <<"\n";

    }
    else if (argc == 2 && argv[0] == "cd")
    {

      if (!changeDirectory(argv[1]))
      {
        std::cout << "cd: " << argv[1] << ": No such file or directory\n";
      }

    }
    else
    {
          std::string pathOutput;
          if (checkPath(pathOutput, argv[0]))
          {
          std::string arguments;
          // std::cout << arguments << '\n';
          excuteProgram(argv);
          }
          else
          {
            std::cout << input << ": command not found\n";
          }
    }

  }

}


void tokenizer(std::vector<std::string>& argList, char delimter, std::string& input, int& argc)
{
  std::string currentToken;
  bool inSingleQuote = false;
  bool inDoubleQuote = false;
  QUOTE_STATE state = QUOTE_STATE::NORMAL;

  int i = 0;
  int length = input.size();
  std::cout << "length is " << length << std::endl;

  while( i < length)
  {
    char c = input[i];

    if (state == QUOTE_STATE::NORMAL)
    {
      if (c == SINGLE_QUOTE)
      {
        inSingleQuote = true;
        state = QUOTE_STATE::IN_SINGLE_QUOTE;
      }
      else if (c == DOUBLE_QUOTE)
      {
        inDoubleQuote = true;
        state = QUOTE_STATE::IN_DOUBLE_QUOTE;
      }
      else if (c == ' ')
      {
        if (!currentToken.empty())
        {
          argList.push_back(currentToken);
          // std::cout << "found token: "<< currentToken << '\n';
          currentToken.clear();
          argc++;
        }
      }
      else if (c == BACKSLASH)
      {
        if (i+1 < length)
        {
          currentToken += input[i+1];
        }
        i+=2;
        continue;
      }
      else 
      {
        currentToken += c;
      }
    }

    else if (state == QUOTE_STATE::IN_SINGLE_QUOTE)
    {
      if(inSingleQuote)
      {
        if (c == SINGLE_QUOTE)
        {
          inSingleQuote = false;
          // argList.push_back(currentToken);
          // // std::cout << "found token: "<< currentToken << '\n';
          // currentToken.clear();
          // argc++;
        }
        else 
        {
          currentToken.push_back(c);
        }
      }
      else
      {
        if (c == SINGLE_QUOTE)
        {
          inSingleQuote = true;
        }
        else if (c == ' ')
        {
          if (!currentToken.empty())
          {
            argList.push_back(currentToken);
            // std::cout << "found token: "<< currentToken << '\n';
            currentToken.clear();
            argc++;
          }
        }
        else 
        {
          currentToken += c;
        }
      }
    }
    else if (state == QUOTE_STATE::IN_DOUBLE_QUOTE)
    {
      if(inDoubleQuote)
      {
        if (c == DOUBLE_QUOTE)
        {
          inDoubleQuote = false;
          // argList.push_back(currentToken);
          // // std::cout << "found token: "<< currentToken << '\n';
          // currentToken.clear();
          // argc++;
        }
        else if (c == BACKSLASH)
        {
          if (i+1 < length)
          {
            if (isEscape(QUOTE_STATE::IN_DOUBLE_QUOTE, input[i+1]))
            {
              std::cout << "escaping : i= " << i+1 << "\n";
              currentToken += input[i+1];
              i+=2;
              continue;
            }
            else 
            {
              currentToken+= BACKSLASH;
              i++;
              continue;
            }

          }
          currentToken+= BACKSLASH;
          i++;
        }
        else 
        {
          currentToken.push_back(c);
        }
      }
      else
      {
        if (c == DOUBLE_QUOTE)
        {
          inDoubleQuote = true;
        }
        else if (c == ' ')
        {
          if (!currentToken.empty())
          {
            argList.push_back(currentToken);
            // std::cout << "found token: "<< currentToken << '\n';
            currentToken.clear();
            argc++;
          }
        }
        else if (c == BACKSLASH)
        {
        if (i+1 < length)
          {
            if (isEscape(QUOTE_STATE::IN_DOUBLE_QUOTE, input[i+1]))
            {
              std::cout << "escaping : i= " << i+1 << "\n";
              currentToken += input[i+1];
              i+=2;
              continue;
            }
            else 
            {
              currentToken+= BACKSLASH;
              i++;
              continue;
            }

          }
        }

        else 
        {
          currentToken += c;
        }
      }
    }
    i++;
  }

  if (!currentToken.empty())
  {
    argList.push_back(currentToken);
    //  std::cout << "found token: "<< currentToken << '\n';
    argc++;
  }

}

void concatenateString(std::string& output, std::vector<std::string>& arr, int index)
{
  for (int i= index; i<arr.size()-1; i++)
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
  for(const std::string builtin:builtins)
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
    for (int i=0; i < parsed_args.size(); ++i)
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

bool isEscape(QUOTE_STATE state, char c)
{
  if (state == QUOTE_STATE::IN_DOUBLE_QUOTE)
  {
    if (DOUBLE_QUOTES_ESCAPES.find(c) != DOUBLE_QUOTES_ESCAPES.end())
    {
      std::cout << "true\n";
      return true;
    }
      std::cout << "false can not be found: " << c << "\n";
    return false;
  }
  else 
  {
    std::cout << "state unknown " << c << "\n";
    return true;
  }
}