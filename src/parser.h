#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

namespace vcbld::parser
{
 std::string parseTxt(const std::string &line);
 std::vector<std::string> parseList(const std::string &line);
}
#endif // !PARSER_H

