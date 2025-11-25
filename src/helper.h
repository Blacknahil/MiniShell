#include <cstring>
#include <unordered_set>
#include <vector>

enum QUOTE_STATE {NORMAL, IN_SINGLE_QUOTE, IN_DOUBLE_QUOTE};
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
void prepareArgsExternal(std::string& output, std::vector<std::string>& arr, int index);