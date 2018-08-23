#ifndef BUILDER_H
#define BUILDER_H

#include <sstream>
#include <string>

#include "prep.h"

namespace vcbld {
class Builder : public PrepClass {
public:
  explicit Builder(const std::string &buildType);
  Builder(const Builder &) = delete;
  Builder &operator=(const Builder &) = delete;

  void compile();
  void appLink();
  void dylibLink();
  void archive();
  std::string getBldCommands();
  void build();
  void copy();

private:
  std::string _buildType;
  std::string _dbgDir;
  std::string _rlsDir;
  std::string _buildDir;
  std::ostringstream _compileCommand;
  std::ostringstream _appLinkCmnd;
  std::ostringstream _libLinkCmnd;
  std::ostringstream _archiveCmnd;
};
} // namespace vcbld
#endif // !BUILDER_H
