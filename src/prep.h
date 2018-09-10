#ifndef PREP_H
#define PREP_H

#include <string>
#include <sstream>
#include <vector>

#include "pkg.h"

namespace vcbld {
class PrepClass : public PkgClass {
public:
  PrepClass(); // reads all configuration files
  PrepClass(const PrepClass &) = delete;
  PrepClass &operator=(const PrepClass &) = delete;

  void posixify(std::string &path);
  std::string sourceFiles();
  std::string sourceFilesSinPath();
  std::string objPath(const std::string &buildPath);
  std::string headerPaths();
  std::string cmakeOutput();
  std::string stripLibName(const std::string &lib);
  std::string dbgLibPaths();
  std::string rlsLibPaths();
  std::vector<std::string> &fullLibNames();
  std::vector<std::string> &fullDbgLibNames();
  std::vector<std::string> &dbgLocalLibNames();
  std::vector<std::string> &rlsLocalLibNames();
  std::vector<std::string> &winDbgDlls();
  std::vector<std::string> &winRlsDlls();

private:
  bool hasComponents(const std::string &libName);
  std::string findDbgLib(const std::string &line);
  std::string findRlsLib(const std::string &line);
  std::string findWinDbgDll(const std::string &line);
  std::string findWinRlsDll(const std::string &line);

  std::ostringstream _cmakeOutput;
  std::vector<std::string> _boostComponents;
  std::vector<std::string> _QtComponents;
  std::vector<std::string> _fullLibNames;
  std::vector<std::string> _fullDbgLibNames;
  std::vector<std::string> _dbgLocalLibNames;
  std::vector<std::string> _rlsLocalLibNames;
  std::vector<std::string> _windDbgDlls;
  std::vector<std::string> _winRlsDlls;
};
} // namespace vcbld
#endif // !PREP_H
