#ifndef ARGS_H
#define ARGS_H

#include <boost/filesystem.hpp>
#include <string>

namespace vcbld::Args {
void New();
void configure();
void restore();
void build(const std::string &buildType);
void clean();
void run(const std::string &buildType);
void available();
void add(const std::string &pkg);
void remove(const std::string &pkg);
void list();
void search(const std::string &);
void generate();
void vcpkg(const std::string &);

std::string sansTriplet(const std::string &);
bool findPackage(const std::string &);

} // namespace vcbld::Args
#endif // ARGS_H