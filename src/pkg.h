#ifndef PKG_H
#define PKG_H

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "conf.h"

namespace fs = boost::filesystem;

namespace vcbld {

class PkgClass {
public:
  explicit PkgClass(const fs::path &vcbldPath);
  void write();
  void include(const std::string &pkg);
  void remove(const std::string &pkg);
  std::string getVersion(const std::string &pkgName);
  std::string headerPaths();
  std::string getLibName(const std::string &lib);
  std::string dbgLibPaths();
  std::string rlsLibPaths();

  std::vector<std::string> packageName;

  ~PkgClass();

private:
  ConfClass *confClass;
};
} // namespace vcbld

#endif