#include "tokenizer.h"
#include "helper.h"

void tokenizer(std::vector<std::string>& argList, std::string& input, int& argc)
{
  std::string currentToken;
  bool inSingleQuote = false;
  bool inDoubleQuote = false;
  QUOTE_STATE state = QUOTE_STATE::NORMAL;

  int i = 0;
  int length = input.size();

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
        }
        else if (c == BACKSLASH)
        {
          if (i+1 < length)
          {
            if (isEscape(QUOTE_STATE::IN_DOUBLE_QUOTE, input[i+1]))
            {
              currentToken += input[i+1];
              i+=2;
              continue;
            }
          }
          currentToken += BACKSLASH;
        }
        else 
        {
          currentToken += c;
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
              currentToken += input[i+1];
              i+=2;
              continue;
            }
          }
              currentToken += BACKSLASH;
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