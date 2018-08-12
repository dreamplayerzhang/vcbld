#ifndef ARGS_H
#define ARGS_H

#include <boost/filesystem.hpp>
#include <string>

namespace fs = boost::filesystem;

namespace vcbld::args {
void New(const std::string &binType);
void configure(const fs::path &vcbldPath);
void restore(const fs::path &vcbldPath);
void build(const std::string &buildType, const fs::path &vcbldPath);
void clean(const fs::path &vcbldPath);
void run(const std::string &buildType, const fs::path &vcbldPath);
void available(const fs::path &vcbldPath);
void add(const std::string &pkg, const fs::path &vcbldPath);
void remove(const std::string &pkg, const fs::path &vcbldPath);
void list(const fs::path &vcbldPath);
void search(const std::string &pkg, const fs::path &vcbldPath);
void generate(const fs::path &vcbldPath);
void vcpkg(const std::string &vcpkgCmnds, const fs::path &vcbldPath);
void cmake(const std::string &cmakeCmnds, const fs::path &vcbldPath);

std::string sinTriplet(const std::string &);
bool findPackage(const std::string &);

} // namespace vcbld::Args
#endif // ARGS_H