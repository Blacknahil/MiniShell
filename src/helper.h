#include <cstring>
#include <unordered_set>
#include <vector>
#include <string>

// Tracks whether the parser is inside a quote.
enum QUOTE_STATE {NORMAL, IN_SINGLE_QUOTE, IN_DOUBLE_QUOTE};
const char SINGLE_QUOTE = '\'';
const char DOUBLE_QUOTE = '\"';
const char BACKSLASH = '\\';
const std::unordered_set<char> DOUBLE_QUOTES_ESCAPES {'\\', '\"'};
struct RedirectOutput 
{
    bool status;
    size_t index;
};

struct ExecuteResult 
{
    int status;
    std::string output;
    std::string error; // Internal errors (pipe, fork, execvp, etc.) are captured here.

};

bool changeDirectory(const std::string& pathStr);
bool checkBuiltin(std::string command);
bool checkPath(std::string& output, const std::string& query);
void concatenateString(std::string& output, std::vector<std::string>& arr, size_t start, size_t end);
void excuteProgram(std::vector<std::string>& parsed_args, size_t end);
bool isEscape(QUOTE_STATE state, char c);
void pwd(std::string& output);
void prepareArgsExternal(std::string& output, std::vector<std::string>& arr, int index);
RedirectOutput checkRedirection(std::vector<std::string>& argList);
void writeToFile(std::string& content, std::string& fileName);
ExecuteResult excuteAndCapture(size_t end,
                               std::vector<std::string>& args);
