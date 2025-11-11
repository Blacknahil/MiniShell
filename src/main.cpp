#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

void splitString(std::vector<std::string>& argList, char delimter, std::string& input, int& argc);
void concatenateString(std::string& output, std::vector<std::string>& arr, int index);

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
