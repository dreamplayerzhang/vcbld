#ifndef INIT_H
#define INIT_H

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace vcbld {
namespace init {
void init(const std::string &binType);
void setup(const fs::path &vcbldPath);
void findPathDirs(std::string &PATH, std::vector<std::string> &dirs);
std::string findCmake(const std::string &dir);
void replaceHome(std::string &path);
void finder(std::vector<std::string> &vector , const std::string &cli);
void posixify(std::string &path);
std::string chooser(std::vector<std::string> &vector , const std::string &cli);

} // namespace init
} // namespace vcbld
#endif // !INIT_H
