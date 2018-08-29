#ifndef CONF_H
#define CONF_H

#include "pch.h"

namespace vcbld {

class ConfClass {
public:
  ConfClass(); // reads all configuration files
  ConfClass(const ConfClass &) = delete;
  ConfClass &operator=(const ConfClass &) = delete;

  std::string projPath() const;
  std::string compilerPath() const;
  std::string cCompilerPath() const;
  std::string cppCompilerPath() const;
  std::string vcpkgDirPath() const;
  std::string architecture() const;
  std::string cmakePath() const;
  std::string makePath() const;
  std::string archiverPath() const;

  std::string projectName() const;
  std::string version() const;
  std::string language() const;
  std::string standard() const;
  std::string binaryName() const;
  std::string binaryType() const;
  std::string outputDirectory() const;
  std::string sourceDirectory() const;
  std::string includeDirectory() const;
  std::string libDirectory() const;
  std::string compilerDefines() const;
  std::string compilerFlags() const;
  std::string linkerFlags() const;

private:
  void posixify(std::string &path);
  
  std::string _projPath;
  std::string _cCompilerPath;
  std::string _cppCompilerPath;
  std::string _vcpkgDirectory;
  std::string _architecture;
  std::string _cmakePath;
  std::string _makePath;
  std::string _archiverPath;
  std::string _projectName;
  std::string _version;
  std::string _language;
  std::string _standard;
  std::string _binaryType;
  std::string _binaryName;
  std::string _outputDirectory;
  std::string _sourceDirectory;
  std::string _includeDirectory;
  std::string _libDirectory;
  std::ostringstream _compilerDefines;
  std::ostringstream _compilerFlags;
  std::ostringstream _linkerFlags;
};
} // namespace vcbld
#endif // !CONF_H
