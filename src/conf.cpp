#include "conf.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <string>

#include "init.h"

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

namespace vcbld {

ConfClass::ConfClass() {
  json vcbldJson, confJson;
  this->_projPath = fs::current_path().string();
  if (this->_projPath.find("\\") != std::string::npos)
    this->_projPath.replace(this->_projPath.begin(), this->_projPath.end(),
                            "\\", "/");

  if (!fs::exists("vcbld.json")) {
    std::cout << "vcbld.json not found in the current directory" << std::endl;
    std::exit(1);
  }

  try {
    std::ifstream vcbldInput("vcbld.json");
    if (vcbldInput.is_open()) {
      vcbldJson = json::parse(vcbldInput);
      vcbldInput.close();
    } else {
      std::cerr << "Failed to open vcbld.json file : " << errno << std::endl;
    }

    this->_projectName = vcbldJson["projectName"];

    try {
      this->_version = vcbldJson["version"];
    } catch (...) {
      this->_version = "0.1.0";
    }

    try {
      this->_language = vcbldJson["language"];
    } catch (...) {
      this->_language = "c++";
    }

    try {
      this->_standard = vcbldJson["standard"];
    } catch (...) {
      this->_standard = "11";
    }

    this->_binaryName = vcbldJson["binaryName"];
    this->_binaryType = vcbldJson["binaryType"];

    try {
      std::string temp =
          fs::canonical(vcbldJson["outputDirectory"].get<std::string>());
      if (temp.find("\\") != std::string::npos)
        temp.replace(temp.begin(), temp.end(), "\\", "/");
      this->_outputDirectory = temp;
    } catch (...) {
      this->_outputDirectory = "./bin";
    }

    try {
      std::string temp =
          fs::canonical(vcbldJson["sourceDirectory"].get<std::string>());
      if (temp.find("\\") != std::string::npos)
        temp.replace(temp.begin(), temp.end(), "\\", "/");
      this->_sourceDirectory = temp;
    } catch (...) {
      this->_sourceDirectory = "./src";
    }

    try {
      std::string temp =
          fs::canonical(vcbldJson["includeDirectory"].get<std::string>());
      if (temp.find("\\") != std::string::npos)
        temp.replace(temp.begin(), temp.end(), "\\", "/");
      this->_includeDirectory = temp;
    } catch (...) {
      this->_includeDirectory = "";
    }

    try {
      std::string temp =
          fs::canonical(vcbldJson["libDirectory"].get<std::string>());
      if (temp.find("\\") != std::string::npos)
        temp.replace(temp.begin(), temp.end(), "\\", "/");
      this->_libDirectory = temp;
    } catch (...) {
      this->_libDirectory = "";
    }

    for (json::iterator it = vcbldJson["compilerDefines"].begin();
         it != vcbldJson["compilerDefines"].end(); ++it) {
      this->_compilerDefines << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["compilerFlags"].begin();
         it != vcbldJson["compilerFlags"].end(); ++it) {
      this->_compilerFlags << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["linkerFlags"].begin();
         it != vcbldJson["linkerFlags"].end(); ++it) {
      this->_linkerFlags << " " << *it << " ";
    }
  } catch (...) {
    std::cerr << "Error reading vcbld.json. " << std::endl;
  }

  try {
    std::ifstream confInput("conf.json");
    if (confInput.is_open()) {
      confJson = json::parse(confInput);
      confInput.close();
    } else {
      std::cerr << "Failed to open conf.json file : " << errno << std::endl;
    }
  } catch (...) {
    std::cerr << "Error reading conf.json." << std::endl;
  }

  this->_vcpkgDirectory =
      fs::canonical(confJson["vcpkgDirectory"].get<std::string>()).string();
  if (this->_vcpkgDirectory.find("\\") != std::string::npos)
    this->_vcpkgDirectory.replace(this->_vcpkgDirectory.begin(),
                                  this->_vcpkgDirectory.end(), "\\", "/");
  this->_cCompilerPath =
      fs::canonical(confJson["cCompilerPath"].get<std::string>()).string();
  if (this->_cCompilerPath.find("\\") != std::string::npos)
    this->_cCompilerPath.replace(this->_cCompilerPath.begin(),
                                 this->_cCompilerPath.end(), "\\", "/");
  this->_cppCompilerPath =
      fs::canonical(confJson["cppCompilerPath"].get<std::string>()).string();
  if (this->_cppCompilerPath.find("\\") != std::string::npos)
    this->_cppCompilerPath.replace(this->_cppCompilerPath.begin(),
                                   this->_cppCompilerPath.end(), "\\", "/");
  this->_architecture = confJson["architecture"].get<std::string>();
  this->_cmakePath =
      fs::canonical(confJson["cmakePath"].get<std::string>()).string();
  if (this->_cmakePath.find("\\") != std::string::npos)
    this->_cmakePath.replace(this->_cmakePath.begin(), this->_cmakePath.end(),
                             "\\", "/");
  this->_makePath =
      fs::canonical(confJson["makePath"].get<std::string>()).string();
  if (this->_makePath.find("\\") != std::string::npos)
    this->_makePath.replace(this->_makePath.begin(), this->_makePath.end(),
                            "\\", "/");
  this->_archiverPath =
      fs::canonical(confJson["archiverPath"].get<std::string>()).string();
  if (this->_archiverPath.find("\\") != std::string::npos)
    this->_archiverPath.replace(this->_archiverPath.begin(),
                                this->_archiverPath.end(), "\\", "/");
}

std::string ConfClass::projPath() const { return this->_projPath; }

std::string ConfClass::compilerPath() const {
  if (this->_language.find("++") != std::string::npos) {
    return this->_cppCompilerPath;
  } else {
    return this->_cCompilerPath;
  }
}
std::string ConfClass::compilerDefines() const {
  return this->_compilerDefines.str();
}
std::string ConfClass::compilerFlags() const {
  return this->_compilerFlags.str();
}
std::string ConfClass::linkerFlags() const { return this->_linkerFlags.str(); }

std::string ConfClass::cmakePath() const { return _cmakePath; }
std::string ConfClass::makePath() const { return _makePath; }
std::string ConfClass::archiverPath() const { return _archiverPath; }
std::string ConfClass::projectName() const { return _projectName; }
std::string ConfClass::version() const { return _version; }
std::string ConfClass::language() const { return _language; }
std::string ConfClass::standard() const { return _standard; }
std::string ConfClass::binaryType() const { return _binaryType; }
std::string ConfClass::binaryName() const { return _binaryName; }
std::string ConfClass::vcpkgDirPath() const { return _vcpkgDirectory; }
std::string ConfClass::outputDirectory() const { return _outputDirectory; }
std::string ConfClass::sourceDirectory() const { return _sourceDirectory; }
std::string ConfClass::includeDirectory() const { return _includeDirectory; }
std::string ConfClass::libDirectory() const { return _libDirectory; }
std::string ConfClass::architecture() const { return _architecture; }
} // namespace vcbld
