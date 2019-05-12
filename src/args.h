#ifndef ARGS_H
#define ARGS_H

#include <string>

#include "fs.h"
#include "init.h"
#include "predefs.h"

namespace vcbld {
namespace args {
void handleArgs(int argc, char **argv);
void setup(Init &init);
std::string parseQuotes(const char *arg);
void New(const BinType&);
void restore();
void build(const Configuration&);
void clean(const Configuration&);
void run(const Configuration&);
void available();
void install(std::vector<std::string> &pkg);
void uninstall(std::vector<std::string> &pkg);
void add(const std::vector<std::string> &pkg);
void remove(const std::vector<std::string> &pkg);
void list();
void find(const std::string &pkg);
void search(const std::string &pkg);
void includes();
void generate();
void commands();
void vcpkg(const std::string &vcpkgCmnds);
void cmake(const std::string &cmakeCmnds);
void make(const std::string &makeArgs);
void help();
std::string sinTriplet(const std::string &);
bool findPackage(const std::string &);
} // namespace args
} // namespace vcbld
#endif // ARGS_H
