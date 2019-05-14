#ifndef INIT_H
#define INIT_H

#include <string>
#include <vector>

#include "fs.h"
#include "predefs.h"

namespace vcbld {
class Init {
public:
  Init() = delete;
  Init(const fs::path &vcbldPath);

  void init(const BinType&);
  void setup();
  void sorter(std::vector<fs::path> &vector);
  void lister(std::vector<fs::path> &vector);
  std::string chooser(std::vector<fs::path> &vector, const size_t &choice);

  void setCompiler(const int &i = 1);
  void setCppCompiler(const int &i = 1);
  void setCmake(const int &i = 1);
  void setMake(const int &i = 1);
  void setArchiver(const int &i = 1);
  void setVcpkg(const int &i = 1);

  std::vector<fs::path> &cCompilers();
  std::vector<fs::path> &cppCompilers();
  std::vector<fs::path> &cmakePaths();
  std::vector<fs::path> &makePaths();
  std::vector<fs::path> &archiverPaths();
  std::vector<fs::path> &vcpkgPaths();

private:
  void findPathDirs(std::string &PATH, std::vector<fs::path> &dirs);
  void replaceHome(fs::path &path);
  void finder(std::vector<fs::path> &vector, const fs::path &dir);
  void posixify(std::string &path);
  fs::path findCmake(const fs::path &path);

  fs::path _vcbldPath;
  std::vector<fs::path> _paths, _cCompilers, _cppCompilers, _cmakePaths,
      _makePaths, _archiverPaths, _vcpkgPaths;
  std::string _cCompilerPath, _cppCompilerPath, _cmakePath, _makePath,
      _archiverPath, _vcpkgPath;
};
} // namespace vcbld
#endif // !INIT_H
