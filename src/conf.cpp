#include "conf.h"

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <algorithm>
#include <errno.h>
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
  _projPath = fs::canonical(fs::current_path()).string();
  posixify(_projPath);

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

    _projectName = vcbldJson["projectName"];

    try {
      _version = vcbldJson["version"];
    } catch (...) {
      _version = "0.1.0";
    }

    try {
      _language = vcbldJson["language"];
    } catch (...) {
      _language = "c++";
    }

    try {
      _standard = vcbldJson["standard"];
    } catch (...) {
      _standard = "11";
    }

    _binaryName = vcbldJson["binaryName"];
    _binaryType = vcbldJson["binaryType"];

    try {
      _outputDirectory =
          fs::canonical(vcbldJson["outputDirectory"].get<std::string>())
              .string();
      posixify(_outputDirectory);
    } catch (...) {
      _outputDirectory = _projPath + "/bin";
    }

    try {
      _sourceDirectory =
          fs::canonical(vcbldJson["sourceDirectory"].get<std::string>())
              .string();
      posixify(_sourceDirectory);
    } catch (...) {
      _sourceDirectory = _projPath + "/src";
    }

    try {
      if (vcbldJson["includeDirectory"].get<std::string>() != "" &&
          fs::exists(vcbldJson["includeDirectory"].get<std::string>())) {
        _includeDirectory =
            fs::canonical(vcbldJson["includeDirectory"].get<std::string>())
                .string();
        posixify(_includeDirectory);
      } else {
        _includeDirectory = "";
      }
    } catch (...) {
      _includeDirectory = "";
    }

    try {
      if (vcbldJson["libDirectory"].get<std::string>() != "" &&
          fs::exists(vcbldJson["libDirectory"].get<std::string>())) {
        _libDirectory =
            fs::canonical(vcbldJson["libDirectory"].get<std::string>())
                .string();
        posixify(_libDirectory);
      } else {
        _libDirectory = "";
      }
    } catch (...) {
      _libDirectory = "";
    }

    for (json::iterator it = vcbldJson["compilerDefines"].begin();
         it != vcbldJson["compilerDefines"].end(); ++it) {
      _compilerDefines << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["compilerFlags"].begin();
         it != vcbldJson["compilerFlags"].end(); ++it) {
      _compilerFlags << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["linkerFlags"].begin();
         it != vcbldJson["linkerFlags"].end(); ++it) {
      _linkerFlags << " " << *it << " ";
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

  _architecture = confJson["architecture"].get<std::string>();

  if (confJson["vcpkgPath"].get<std::string>() != "" &&
      fs::exists(confJson["vcpkgPath"].get<std::string>())) {
    _vcpkgDirectory =
        fs::canonical(confJson["vcpkgPath"].get<std::string>())
            .parent_path()
            .string();
    posixify(_vcpkgDirectory);
  }

  if (confJson["cCompilerPath"].get<std::string>() != "" &&
      fs::exists(confJson["cCompilerPath"].get<std::string>())) {
    _cCompilerPath =
        fs::canonical(confJson["cCompilerPath"].get<std::string>()).string();
    posixify(_cCompilerPath);
  }

  if (confJson["cppCompilerPath"].get<std::string>() != "" &&
      fs::exists(confJson["cppCompilerPath"].get<std::string>())) {
    _cppCompilerPath =
        fs::canonical(confJson["cppCompilerPath"].get<std::string>()).string();
    posixify(_cppCompilerPath);
  }

  if (confJson["cmakePath"].get<std::string>() != "" &&
      fs::exists(confJson["cmakePath"].get<std::string>())) {
    _cmakePath =
        fs::canonical(confJson["cmakePath"].get<std::string>()).string();
    posixify(_cmakePath);
  }

  if (confJson["makePath"].get<std::string>() != "" &&
      fs::exists(confJson["makePath"].get<std::string>())) {
    _makePath =
        fs::canonical(confJson["makePath"].get<std::string>()).string();
    posixify(_makePath);
  }

  if (confJson["archiverPath"].get<std::string>() != "" &&
      fs::exists(confJson["archiverPath"].get<std::string>())) {
    _archiverPath =
        fs::canonical(confJson["archiverPath"].get<std::string>()).string();
    posixify(_archiverPath);
  }
}

std::string ConfClass::projPath() const { return _projPath; }

std::string ConfClass::compilerPath() const {
  if (_language.find("++") != std::string::npos) {
    return _cppCompilerPath;
  } else {
    return _cCompilerPath;
  }
}

std::string ConfClass::cCompilerPath() const { return _cCompilerPath; }

std::string ConfClass::cppCompilerPath() const {
  return _cppCompilerPath;
}

std::string ConfClass::compilerDefines() const {
  return _compilerDefines.str();
}
std::string ConfClass::compilerFlags() const {
  return _compilerFlags.str();
}
std::string ConfClass::linkerFlags() const { return _linkerFlags.str(); }

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
std::string ConfClass::outputDirectory() const {
  return _outputDirectory;
}
std::string ConfClass::sourceDirectory() const {
  return _sourceDirectory;
}
std::string ConfClass::includeDirectory() const {
  return _includeDirectory;
}
std::string ConfClass::libDirectory() const { return _libDirectory; }
std::string ConfClass::architecture() const { return _architecture; }

void ConfClass::posixify(std::string &path) {
  if (path.find("\\") != std::string::npos) {
    path.replace(path.find("\\"), 1, "/");
    posixify(path);
  }
}
} // namespace vcbld
