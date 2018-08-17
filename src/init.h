#ifndef INIT_H
#define INIT_H

#include <experimental/filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace vcbld
{
namespace init
{
void init(const std::string &binType);
void setup(const fs::path &vcbldPath);
std::string findCmake(const std::string &dir);
} // namespace init
} // namespace vcbld
#endif // !INIT_H
