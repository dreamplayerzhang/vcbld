#ifndef PKG_H
#define PKG_H

#include <string>
#include <vector>

#include "conf.h"

namespace vcbld {
class PkgClass : public ConfClass {
public:
  PkgClass(); // reads all configuration files
  PkgClass(const PkgClass &) = delete;
  PkgClass &operator=(const PkgClass &) = delete;

  void write();
  void include(const std::string &pkg);
  void remove(const std::string &pkg);
  std::string getVersion(const std::string &pkgName);
  std::vector<std::string> &packageNames();

private:
  std::vector<std::string> _packageNames;
};
} // namespace vcbld
#endif // !PKG_H
