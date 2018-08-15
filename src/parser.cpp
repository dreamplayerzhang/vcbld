#include "parser.h"

namespace vcbld::parser
{
std::string parseTxt(const std::string &line)
{
    std::size_t foundColon;
    std::string cropped;
    foundColon = line.find(": ");
    if (foundColon != std::string::npos)
    {
        cropped = line.substr((foundColon + 2), line.length());
    }
    return cropped;
}

std::vector<std::string> parseList(const std::string &line)
{
    std::vector<std::string> list;
    std::size_t len = line.length();
    for (int i = 0, j = 0; i < len; i++)
    {
        if (line[i] == ' ')
        {
            std::string ch = line.substr(j, i - j);
            j = i + 1;
            list.push_back(ch);
        }
        if (i == len - 1) 
        {
            std::string ch = line.substr(j, i - (j + 1));
            list.push_back(ch);
        }
    }
    return list;
}
} // namespace vcbld::parser
