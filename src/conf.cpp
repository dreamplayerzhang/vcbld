#include "conf.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <vector>

#if defined(_WIN32)
#define PLATFORM_NAME "x86-windows"
#define PATHSEP "\"
#elif defined(_WIN64)
#define PLATFORM_NAME "x64-windows"
#define PATHSEP "\"
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_NAME "x64-windows"
#define PATHSEP "/"
#elif defined(__linux__)
#define PLATFORM_NAME "x64-linux"
#define PATHSEP "/"
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC == 1
#define PLATFORM_NAME "x64-osx" // Apple OSX
#define PATHSEP "/"
#endif
#else
#define PLATFORM_NAME NULL
#endif

using json = nlohmann::json;
namespace fs = boost::filesystem;

namespace vcbld {

ConfClass::ConfClass() {
  json confJson, vcbldJson;
  this->_selfPath = fs::current_path();

  try {
    std::ifstream confInput("conf.json");
    if (confInput.is_open()) {
      confJson = json::parse(confInput);
      confInput.close();
    } else {
      std::cerr << "Failed to open conf.json file : " << errno << std::endl;
    }
  } catch (const json::parse_error e) {
    std::cerr << "Error reading conf.json." << std::endl;
  }

  this->_cCompilerPath = confJson["cCompilerPath"];
  this->_cppCompilerPath = confJson["cppCompilerPath"];
  this->_vcpkgDirPath = confJson["vcpkgDirectory"];
  this->_architecture = confJson["architecture"];

  try {
    std::ifstream vcbldInput("vcbld.json");
    if (vcbldInput.is_open()) {
      vcbldJson = json::parse(vcbldInput);
      vcbldInput.close();
    } else {
      std::cerr << "Failed to open vcbld.json file : " << errno << std::endl;
    }
    this->_projectName = vcbldJson["projectName"];
    this->_version = vcbldJson["version"];
    this->_language = vcbldJson["language"];
    this->_binaryName = vcbldJson["binaryName"];
    this->_binaryType = vcbldJson["binaryType"];
    this->_outputDirectory = vcbldJson["outputDirectory"];
    this->_sourceDirectory = vcbldJson["sourceDirectory"];
    this->_includeDirectory = vcbldJson["includeDirectory"];
    this->_libsDirectory = vcbldJson["libsDirectory"];

    for (json::iterator it = vcbldJson["compilerDefines"].begin();
         it != vcbldJson["compilerDefines"].end(); ++it) {
      this->_compilerDefines << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["compilerFlags"].begin();
         it != vcbldJson["compilerFlags"].end(); ++it) {
      this->_compilerFlags << " " << *it << " ";
    }
  } catch (const json::parse_error e) {
    std::cerr << "Error reading vcbld.json." << std::endl;
  }
}

std::string ConfClass::sourceFiles() const {
  std::vector<fs::directory_entry> v;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = this->_sourceDirectory;
  if (fs::is_directory((fs::path)tempPath)) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator it = v.begin();
         it != v.end(); ++it) {
      if (fs::extension((*it).path().filename().string()) == ".cpp" ||
          fs::extension((*it).path().filename().string()) == ".rc" ||
          fs::extension((*it).path().filename().string()) == ".c" ||
          fs::extension((*it).path().filename().string()) == ".cxx") {
        temp << this->_selfPath.c_str() << PATHSEP << tempPath + PATHSEP
             << (*it).path().filename().string() << " ";
      }
    }
  }
  return temp.str();
}

std::string ConfClass::compilerPath() const {
  if (this->_language.at(2) == '+') {
    return this->_cppCompilerPath;
  } else {
    return this->_cCompilerPath;
  }
}

std::string ConfClass::projectName() const { return _projectName; }
std::string ConfClass::version() const { return _version; }
std::string ConfClass::language() const { return _language; }
std::string ConfClass::binaryType() const { return _binaryType; }
std::string ConfClass::binaryName() const { return _binaryName; }
std::string ConfClass::vcpkgDirPath() const { return _vcpkgDirPath; }
fs::path ConfClass::outputDirectory() const { return _outputDirectory; }
fs::path ConfClass::sourceDirectory() const { return _sourceDirectory; }
fs::path ConfClass::includeDirectory() const { return _includeDirectory; }
fs::path ConfClass::libsDirectory() const { return _libsDirectory; }
std::string ConfClass::architecture() const { return _architecture; }
std::string ConfClass::compilerDefines() const {
  return this->_compilerDefines.str();
}
std::string ConfClass::compilerFlags() const {
  return this->_compilerFlags.str();
}

} // namespace vcbld