#ifndef PREP_H
#define PREP_H

#include <sstream>
#include <string>
#include <vector>

#include "pkg.h"

namespace vcbld {
class PrepClass : public PkgClass {
public:
  PrepClass(); // reads all configuration files
  PrepClass(const ConfClass &) = delete;
  PrepClass &operator=(const ConfClass &) = delete;

  std::string sourceFiles();
  std::string sourceFilesSinPath();
  std::string objPath(const std::string &buildPath);
  std::vector<std::string> &fullLibNames();
  std::vector<std::string> &dbgLocalLibNames();
  std::vector<std::string> &rlsLocalLibNames();
  std::string headerPaths();
  std::string findLib(const std::string &line);
  std::string stripLibName(const std::string &lib);
  std::string dbgLibPaths();
  std::string rlsLibPaths();

  bool hasComponents(const std::string &libName);
  std::string cmakeOutput();
  void posixify(std::string &path);

private:
  std::ostringstream _cmakeOutput;
  std::vector<std::string> _boostComponents;
  std::vector<std::string> _QtComponents;
  std::vector<std::string> _fullLibNames;
  std::vector<std::string> _dbgLocalLibNames;
  std::vector<std::string> _rlsLocalLibNames;
};
} // namespace vcbld
#endif // !PREP_H
