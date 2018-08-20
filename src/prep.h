#ifndef PREP_H
#define PREP_H

#include <sstream>
#include <string>
#include <vector>

#include "pkg.h"

namespace vcbld
{
class PrepClass : public PkgClass
{
  public:
    PrepClass(); // reads all configuration files
    PrepClass(const ConfClass &) = delete;
    PrepClass &operator=(const ConfClass &) = delete;

    std::string sourceFiles() const;
    std::string sourceFilesSinPath() const;
    std::vector<std::string> &fullLibNames();
    std::string headerPaths();
    std::string findLib(const std::string &line);
    std::string stripLibName(const std::string &lib);
    std::string dbgLibPaths();
    std::string rlsLibPaths();

    bool hasComponents(const std::string &libName);
    std::string cmakeOutput();

  private:
    std::ostringstream _cmakeOutput;
    std::vector<std::string> _boostComponents;
    std::vector<std::string> _QtComponents;
    std::vector<std::string> _fullLibNames;
};
} // namespace vcbld
#endif // !PREP_H