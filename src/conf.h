#ifndef CONF_H
#define CONF_H

#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

namespace vcbld
{

class ConfClass
{
public:
  explicit ConfClass(const fs::path &vcbldPath); // reads vcbld.json

  std::string compilerPath() const;
  std::string vcpkgDirPath() const;
  std::string architecture() const;
  std::string cmakePath() const;

  std::string projectName() const;
  std::string version() const;
  std::string language() const;
  std::string standard() const;
  std::string binaryName() const;
  std::string binaryType() const;
  fs::path outputDirectory() const;
  fs::path sourceDirectory() const;
  fs::path includeDirectory() const;
  fs::path libsDirectory() const;
  std::string sourceFiles() const;
  std::string sourceFilesSinPath() const;
  std::string compilerDefines() const;
  std::string compilerFlags() const;

  void write();
  void include(const std::string &pkg);
  void remove(const std::string &pkg);
  std::string getVersion(const std::string &pkgName);
  std::string headerPaths();
  std::string stripLibName(const std::string &lib);
  std::string dbgLibPaths();
  std::string rlsLibPaths();

  bool hasComponents(const std::string &libName);
  std::string cmakeOutput() const;

  std::vector<std::string> packageNames;
  std::vector<std::string> libs;

private:
  fs::path _vcbldPath;
  fs::path _projPath;
  std::string _cCompilerPath;
  std::string _cppCompilerPath;
  std::string _vcpkgDirectory;
  std::string _vcpkgDirPath;
  std::string _architecture;
  std::string _cmakePath;
  std::string _projectName;
  std::string _version;
  std::string _language;
  std::string _standard;
  std::string _binaryType;
  std::string _binaryName;
  std::string _outputDirectory;
  std::string _sourceDirectory;
  std::string _includeDirectory;
  std::string _libsDirectory;
  std::ostringstream _compilerDefines;
  std::ostringstream _compilerFlags;

  std::ostringstream _cmakeOutput;
  std::vector<std::string> boostComponents;
  std::vector<std::string> QtComponents;
};
} // namespace vcbld

#endif // !CONF_H