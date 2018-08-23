#include "conf.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
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
  this->_projPath = fs::canonical(fs::current_path()).string();
  posixify(this->_projPath);

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
      this->_outputDirectory = fs::canonical(vcbldJson["outputDirectory"].get<std::string>()).string();
      posixify(this->_outputDirectory);
    } catch (...) {
      this->_outputDirectory = this->_projPath + "/bin";
    }

    try {
      this->_sourceDirectory = fs::canonical(vcbldJson["sourceDirectory"].get<std::string>()).string();
      posixify(this->_sourceDirectory);
    } catch (...) {
      this->_sourceDirectory = this->_projPath + "/src";
    }

    try {
      if (vcbldJson["includeDirectory"].get<std::string>() != "" && fs::exists(vcbldJson["includeDirectory"].get<std::string>())) {
      this->_includeDirectory = fs::canonical(vcbldJson["includeDirectory"].get<std::string>()).string();
      posixify(this->_includeDirectory);
      } else {
        this->_includeDirectory = "";
      }
    } catch (...) {
      this->_includeDirectory = "";
    }

    try {
      if (vcbldJson["libDirectory"].get<std::string>() != "" && fs::exists(vcbldJson["libDirectory"].get<std::string>())) {
      this->_libDirectory = fs::canonical(vcbldJson["libDirectory"].get<std::string>()).string();
      posixify(this->_libDirectory);
      } else {
        this->_libDirectory = "";
      }
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

  this->_architecture = confJson["architecture"].get<std::string>();

  if (confJson["vcpkgDirectory"].get<std::string>() != "" && fs::exists(vcbldJson["vcpkgDirectory"].get<std::string>())) {
    this->_vcpkgDirectory =
      fs::canonical(confJson["vcpkgDirectory"].get<std::string>()).string();
    posixify(this->_vcpkgDirectory);
  }

  if (confJson["cCompilerPath"].get<std::string>() != "" && fs::exists(vcbldJson["cCompilerPath"].get<std::string>())) {
  this->_cCompilerPath =
      fs::canonical(confJson["cCompilerPath"].get<std::string>()).string();
  posixify(this->_cCompilerPath);
  }

  if (confJson["cppCompilerPath"].get<std::string>() != "" && fs::exists(vcbldJson["cppCompilerPath"].get<std::string>())) {
  this->_cppCompilerPath =
      fs::canonical(confJson["cppCompilerPath"].get<std::string>()).string();
  posixify(this->_cppCompilerPath);
  }
  
  if (confJson["cmakePath"].get<std::string>() != "" && fs::exists(vcbldJson["cmakePath"].get<std::string>())) {
  this->_cmakePath =
      fs::canonical(confJson["cmakePath"].get<std::string>()).string();
  posixify(this->_cmakePath);
  }

  if (confJson["makePath"].get<std::string>() != "" && fs::exists(vcbldJson["makePath"].get<std::string>())) {
  this->_makePath =
      fs::canonical(confJson["makePath"].get<std::string>()).string();
  posixify(this->_cmakePath);
  }

  if (confJson["archiverPath"].get<std::string>() != ""  && fs::exists(vcbldJson["archiverPath"].get<std::string>())) {
  this->_archiverPath =
      fs::canonical(confJson["archiverPath"].get<std::string>()).string();
  posixify(this->_archiverPath);
  }
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

void ConfClass::posixify(std::string &path) {
  if (path.find("\\") != std::string::npos) {
    path.replace(path.find("\\"), 1, "/");
    posixify(path);
  }
}
} // namespace vcbld
