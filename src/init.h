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
void replaceHome(std::string &path);
std::string findCmake(const std::string &dir);
std::string cCompiler(std::vector<std::string> &cCompilers);
std::string cppCompiler(std::vector<std::string> &cppCompilers);
std::string cmake(std::vector<std::string> &cmakes);
std::string make(std::vector<std::string> &makes);
std::string archiver(std::vector<std::string> &archivers);
std::string vcpkg(std::vector<std::string> &vcpkgs);
void posixify(std::string &path);
} // namespace init
} // namespace vcbld
#endif // !INIT_H
