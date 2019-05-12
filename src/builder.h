#ifndef BUILDER_H
#define BUILDER_H

#include <sstream>
#include <string>

#include "predefs.h"
#include "prep.h"

namespace vcbld {
class Builder : public PrepClass {
public:
  Builder() = delete;
  explicit Builder(const Configuration&);
  Builder(const Builder &) = delete;
  Builder &operator=(const Builder &) = delete;

  void build();
  void copy();
  std::string objPath(const std::string &buildPath);
  std::string compileCommand() const;
  std::string appLinkCmnd() const;
  std::string libLinkCmnd() const;
  std::string archiveCmnd() const;
  void compile();
  void appLink();
  void dylibLink();
  void archive();
  std::string getBldCommands();

private:
  std::string _buildType;
  std::string _dbgDir;
  std::string _rlsDir;
  std::string _buildDir;
  std::ostringstream _compileCommand;
  std::ostringstream _appLinkCmnd;
  std::ostringstream _libLinkCmnd;
  std::ostringstream _archiveCmnd;
  void exec(const std::string &);
  size_t _outCount;
};
} // namespace vcbld
#endif // !BUILDER_H
