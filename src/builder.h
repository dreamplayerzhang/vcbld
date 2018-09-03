#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <sstream>

#include "prep.h"

namespace vcbld {
class Builder : public PrepClass {
public:
  Builder() = delete;
  explicit Builder(const std::string &buildType);
  Builder(const Builder &) = delete;
  Builder &operator=(const Builder &) = delete;

  void build();
  void copy();
  std::string getBldCommands();

private:
  void compile();
  void appLink();
  void dylibLink();
  void archive();

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
