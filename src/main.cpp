#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unistd.h>
#include <vector>


const char* ENV_VAR_NAME = "PATH";

void splitString(std::vector<std::string>& argList, char delimter, std::string& input, int& argc);
void concatenateString(std::string& output, std::vector<std::string>& arr, int index);
bool checkPath(std::string& output, const std::string& query);


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
    splitString(argv, ' ', input,argc);
    // std::cout << "size : " << argc <<  "argv[0]"<< argv[0];
    if (argc ==2 && argv[0] == "exit")
    {
      if (argv[1] == "0")
      {
        return 0;
      }
      return 1;
    }
    else if (argc >=2 && argv[0]=="echo")
    {
      std::string output;
      concatenateString(output, argv, 1);
      std::cout << output <<"\n";
    }
    else if (argc >= 2 && argv[0] == "type")
    {
      if (argv[1] == "type" || argv[1] =="echo" || argv[1] == "exit")
      {
        std::cout << argv[1] << " is a shell builtin\n";
      }
      else 
      {
        std::string concatenated;
        concatenateString(concatenated, argv, 1);
        std::string pathOutput;

        if (checkPath(pathOutput,concatenated))
        {
          std::cout << concatenated << " is " << pathOutput << "\n";

        }
        else 
        {
           std::cout << concatenated <<": not found\n";
        }
       
      }
    }
    else
    {
          std::cout << input << ": command not found\n";
    }

  }

}


void splitString(std::vector<std::string>& argList, char delimter, std::string& input, int& argc)
{
  std::istringstream stream(input);

  std::string token;

  while (std::getline(stream, token, delimter))
  {
    argList.push_back(token);
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
