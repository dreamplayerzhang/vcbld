#include "conf.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <vector>

#include "init.h"

using json = nlohmann::json;
namespace fs = boost::filesystem;

namespace vcbld {

ConfClass::ConfClass(const fs::path &vcbldPath) {
  json vcbldJson, confJson, pkgsJson;
  this->_vcbldPath = vcbldPath;
  this->_projPath = fs::current_path();

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
    this->_standard = vcbldJson["standard"];
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

  std::string confJsonPath = vcbldPath.string() + "/" + "conf.json";
  if (!fs::exists(confJsonPath)) {
    init::setup(this->_vcbldPath);
  }

  try {
    std::ifstream confInput(confJsonPath);
    if (confInput.is_open()) {
      confJson = json::parse(confInput);
      confInput.close();
    } else {
      std::cerr << "Failed to open conf.json file : " << errno << std::endl;
    }
  } catch (const json::parse_error e) {
    std::cerr << "Error reading conf.json." << std::endl;
  }

  fs::path vcpkgPath;
  this->_vcpkgDirectory = confJson["vcpkgDirectory"];
  vcpkgPath = static_cast<std::string>(this->_vcpkgDirectory);
  this->_cCompilerPath = confJson["cCompilerPath"];
  this->_cppCompilerPath = confJson["cppCompilerPath"];
  this->_architecture = confJson["architecture"];
  this->_cmakePath = confJson["cmakePath"];
  this->_vcpkgDirPath = vcpkgPath.string();

  try {
    std::ifstream pkgsInput("packages.json");
    if (pkgsInput.is_open()) {
      pkgsJson = json::parse(pkgsInput);
      pkgsInput.close();
    } else {
      std::cerr << "Failed to open packages.json file : " << errno << std::endl;
    }
    for (json::iterator it = pkgsJson["packages"].begin();
         it != pkgsJson["packages"].end(); ++it) {
      this->packageName.push_back(*it);
    }
  } catch (const json::parse_error e) {
    std::cerr << "Error reading packages.json" << std::endl;
  }
}

void ConfClass::write() {
  json pkgsJson;

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    pkgsJson["packages"].push_back(*it);
  }
  std::ofstream pkgsOutput("packages.json");
  if (pkgsOutput.is_open()) {
    pkgsOutput << std::setw(4) << pkgsJson;
    pkgsOutput.flush();
    pkgsOutput.close();
    std::cout << "Packages updated successfully." << std::endl;
  } else {
    std::cout << "Packages updated successfully." << std::endl;
    std::cerr << "Failed to open file : " << errno << std::endl;
  }
  std::string line;
  bool isEmpty = false;
  std::ifstream pkgsInput("packages.json");
  if (pkgsInput.is_open()) {
    std::getline(pkgsInput, line);
    if (line == "null") {
      isEmpty = true;
      pkgsInput.close();
    }
  } else {
    std::cout << "Packages updated successfully." << std::endl;
    std::cerr << "Failed to open file : " << errno << std::endl;
  }
  if (isEmpty == true) {
    fs::remove("packages.json");
    init::init(this->binaryType());
  }
}

std::string ConfClass::sourceFiles() const {
  std::vector<fs::directory_entry> v;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = this->_sourceDirectory;
  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator it = v.begin();
         it != v.end(); ++it) {
      if (fs::extension((*it).path().filename().string()) == ".cpp" ||
          fs::extension((*it).path().filename().string()) == ".rc" ||
          fs::extension((*it).path().filename().string()) == ".c" ||
          fs::extension((*it).path().filename().string()) == ".cxx") {
        temp << this->_projPath.c_str() << "/" << tempPath + "/"
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

std::string ConfClass::getVersion(const std::string &pkgName) {
  std::string ctrlPath = this->vcpkgDirPath() + "/" + "packages" +
                         "/" + pkgName + "_" + this->architecture() +
                         "/" + "CONTROL";
  std::string line;
  try {
    std::ifstream input(ctrlPath);
    if (input.is_open()) {
      std::getline(input, line);
      std::getline(input, line);

      input.close();
    } else {
      std::cerr << "Failed to open file : " << errno << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "CONTROL file not found" << std::endl;
  }
  std::size_t found;
  std::string cropVersion;
  found = line.find(": ");
  if (found != std::string::npos) {
    cropVersion = line.substr((found + 2), line.length());
  }
  return cropVersion;
}

std::string ConfClass::headerPaths() {
  std::ostringstream temp;
  temp << " -I" << this->sourceDirectory() << " -I"
       << this->includeDirectory() << " -I"
       << this->vcpkgDirPath() << "/"
       << "installed"
       << "/" << this->architecture() << "/"
       << "include";

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    temp << " -I" << this->vcpkgDirPath() << "/"
         << "packages"
         << "/" << *it << "_" << this->architecture() << "/"
         << "/";
  }
  return temp.str();
}

std::string ConfClass::getLibName(const std::string &lib) {
  std::size_t found;
  std::string stripLib;
  std::string stripExt;
  found = lib.find("lib");
  if (found != std::string::npos && found == 0) {
    stripLib = lib.substr(3, lib.length());
  }
  found = stripLib.find(".a");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 2));
  }
  found = stripLib.find(".so");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 3));
  }
  found = stripLib.find(".dylib");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 6));
  }
  found = stripLib.find(".dll");
  if (found != std::string::npos) {
    stripExt = stripLib.substr(0, (stripLib.length() - 4));
  }

  return stripExt;
}

std::string ConfClass::dbgLibPaths() {
  std::ostringstream temp;
  std::string localDbgLibs =
      this->libsDirectory().string() + "/" + "debug";
  std::vector<fs::directory_entry> v;

  if (fs::is_directory(static_cast<fs::path>(localDbgLibs))) {
    std::copy(fs::directory_iterator(localDbgLibs), fs::directory_iterator(),
              std::back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator jt = v.begin();
         jt != v.end(); ++jt) {
      if (getLibName((*jt).path().filename().string()).at(0) != '.') {
        temp << " -L" << (*jt).path().parent_path().string() << " "
             << " -l" << getLibName((*jt).path().filename().string());
      }
    }
  }

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    std::string vcpkgDbgLibs =
        this->vcpkgDirPath() + "/" + "packages" + "/" + *it + "_" +
        this->architecture() + "/" + "debug" + "/" + "lib";
    if (fs::is_directory(static_cast<fs::path>(vcpkgDbgLibs))) {
      std::copy(fs::directory_iterator(vcpkgDbgLibs), fs::directory_iterator(),
                std::back_inserter(v));
      for (std::vector<fs::directory_entry>::iterator jt = v.begin();
           jt != v.end(); ++jt) {
        if (getLibName((*jt).path().filename().string()).at(0) != '.') {
          temp << " -L" << (*jt).path().parent_path().string() << " "
               << " -l" << getLibName((*jt).path().filename().string());
        }
      }
    }
  }
  return temp.str();
}
std::string ConfClass::rlsLibPaths() {
  std::ostringstream temp;
  std::string localRlsLibs =
      this->libsDirectory().string() + "/" + "lib";
  std::vector<fs::directory_entry> v;

  if (fs::is_directory(static_cast<fs::path>(localRlsLibs))) {
    std::copy(fs::directory_iterator(localRlsLibs), fs::directory_iterator(),
              std::back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator jt = v.begin();
         jt != v.end(); ++jt) {
      if (getLibName((*jt).path().filename().string()).at(0) != '.') {
        temp << " -L" << (*jt).path().parent_path().string() << " "
             << " -l" << getLibName((*jt).path().filename().string());
      }
    }
  }

  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end(); ++it) {
    std::string vcpkgRlsLibs = this->vcpkgDirPath() + "/" +
                               "packages" + "/" + *it + "_" +
                               this->architecture() + "/" + "lib";
    if (fs::is_directory(static_cast<fs::path>(vcpkgRlsLibs))) {
      std::copy(fs::directory_iterator(vcpkgRlsLibs), fs::directory_iterator(),
                std::back_inserter(v));
      for (std::vector<fs::directory_entry>::iterator jt = v.begin();
           jt != v.end(); ++jt) {
        if (getLibName((*jt).path().filename().string()).at(0) != '.') {
          temp << " -L" << (*jt).path().parent_path().string() << " "
               << " -l" << getLibName((*jt).path().filename().string());
        }
      }
    }
  }
  return temp.str();
}

void ConfClass::include(const std::string &pkgName) {
  this->packageName.push_back(pkgName);
}

void ConfClass::remove(const std::string &pkgName) {
  for (std::vector<std::string>::iterator it = this->packageName.begin();
       it != this->packageName.end();) {
    if (*it == pkgName) {
      it = this->packageName.erase(it);
    } else {
      ++it;
    }
  }
}
std::string ConfClass::cmakePath() const { return _cmakePath; }
std::string ConfClass::projectName() const { return _projectName; }
std::string ConfClass::version() const { return _version; }
std::string ConfClass::language() const { return _language; }
std::string ConfClass::standard() const { return _standard; }
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