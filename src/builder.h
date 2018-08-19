#ifndef BUILDER_H
#define BUILDER_H

#include <string>

#include "conf.h"

namespace vcbld
{
class Builder
{
public:
  explicit Builder(const std::string &buildType);
  Builder(const Builder &) = delete;
  Builder &operator=(const Builder &) = delete;
  ~Builder();

  void compile();
  void appLink();
  void dylibLink();
  void archive();
  std::string getBldCommands();
  void build();
  void copy();

private:
  ConfClass *confClass;
  std::string _buildType;
  std::string _dbgDir;
  std::string _rlsDir;
  std::ostringstream _compileCommand;
  std::ostringstream _appLinkCmnd;
  std::ostringstream _libLinkCmnd;
  std::ostringstream _archiveCmnd;
};
} // namespace vcbld
#endif // !BUILDER_H
