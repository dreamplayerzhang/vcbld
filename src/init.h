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
void findPathDirs(std::string &PATH, std::vector<fs::path> &dirs);
void replaceHome(fs::path &path);
void finder(std::vector<fs::path> &vector , const fs::path &dir);
void posixify(std::string &path);
fs::path findCmake(const fs::path &path);
std::string chooser(std::vector<fs::path> &vector , const std::string &cli);
} // namespace init
} // namespace vcbld
#endif // !INIT_H
