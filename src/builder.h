#ifndef BUILDER_H
#define BUILDER_H

#include <boost/filesystem.hpp>
#include <string>

#include "conf.h"
#include "pkg.h"

namespace vcbld {
class Builder {
public:
  Builder(const char *buildType);

  std::string compile();
  std::string appLink();
  std::string dylibLink();
  std::string archive();
  std::string getBldCommands();
  void build();

private:
  ConfClass confClass;
  PkgClass pkgClass;
  const char *_buildType;
  std::string _dbgDir;
  std::string _rlsDir;
  std::ostringstream _compileCommand;
  std::ostringstream _appLinkCmnd;
  std::ostringstream _libLinkCmnd;
  std::ostringstream _archiveCmnd;
};
} // namespace vcbld

#endif // !BUILDER_H
