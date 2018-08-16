#ifndef ARGS_H
#define ARGS_H

#include <string>
#include <vector>

namespace vcbld
{
namespace args
{
void New(const std::string &binType);
void restore();
void build(const std::string &buildType);
void clean();
void run(const std::string &buildType);
void available();
void add(const std::vector<std::string> &pkg);
void remove(const std::vector<std::string> &pkg);
void list();
void search(const std::string &pkg);
void includes();
void generate();
void vcpkg(const std::string &vcpkgCmnds);
void cmake(const std::string &cmakeCmnds);
void make(const std::string &makeArgs);

std::string sinTriplet(const std::string &);
bool findPackage(const std::string &);
} // namespace args
} // namespace vcbld
#endif // ARGS_H
