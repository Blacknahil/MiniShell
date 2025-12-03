#include <cstring>
#include <iostream>
#include <string>

#include "helper.h"
#include "tokenizer.h"




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
    size_t argc = 0;
    tokenizer(argv, input,argc);

    RedirectOutput isRedirect = checkRedirection(argv);

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
      if (isRedirect.status)
      {
        concatenateString(output, argv, 1,isRedirect.index);

        if (isRedirect.index +1 < argc)
        {
          // write to file 
          writeToFile(output, argv[isRedirect.index +1]);
          //  std::cout << output <<"\n";
        }
        else 
        {
           std::cout << output <<"\n";
        }
      }
      else 
      {
        concatenateString(output, argv, 1,argc);
        std::cout << output <<"\n";
      }

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
        concatenateString(concatenated, argv, 1, argc);
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
            std::string output;
            if (isRedirect.status)
            {
              excuteAndCapture(output,isRedirect.index, argv);
              if (isRedirect.index +1 < argc)
              {
                // write to file 
                writeToFile(output, argv[isRedirect.index+1]);
              }
              else
              {
                std::cout << output << "\n";
              }

            }
            else 
            {
              excuteProgram(argv,argc);
              std::cout << std::endl;
            }
        
          }
          else
          {
            std::cout << input << ": command not found\n";
          }
    }

  }
}