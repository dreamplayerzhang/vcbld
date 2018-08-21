#ifndef INIT_H
#define INIT_H

#include <experimental/filesystem>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace vcbld
{
namespace init
{
void init(const std::string &binType);
void setup(const fs::path &vcbldPath);
void findPathDirs(std::string &PATH, std::vector<std::string> &dirs);
void replaceHome(std::string &path);
std::string findCmake(const std::string &dir);
std::string cCompiler(const std::vector<std::string> &cCompilers);
std::string cppCompiler(const std::vector<std::string> &cppCompilers);
std::string cmake(const std::vector<std::string> &cmakes);
std::string make(const std::vector<std::string> &makes);
std::string archiver(const std::vector<std::string> &archivers);
} // namespace init
} // namespace vcbld
#endif // !INIT_H
