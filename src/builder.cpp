#include "builder.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <iterator>
#include <vector>

namespace fs = boost::filesystem;

namespace vcbld {

Builder::Builder(const std::string &buildType, const fs::path &vcbldPath) {
  confClass = new ConfClass(vcbldPath);
  pkgClass = new PkgClass(vcbldPath);
  this->_buildType = buildType;
  if (!fs::exists("vcbld.json")) {
    std::cout << "Build configuration not found!" << std::endl;
    std::exit(1);
  }

  this->_dbgDir = this->confClass->outputDirectory().string() + "/" + "debug";
  this->_rlsDir = this->confClass->outputDirectory().string() + "/" + "release";
}

std::string Builder::compile() {

  if (this->_buildType == "release") {
    this->_compileCommand << "cd " << this->_rlsDir << " && "
                          << this->confClass->compilerPath() << " -c "
                          << this->confClass->sourceFiles() << " "
                          << this->confClass->compilerDefines() << " "
                          << this->confClass->compilerFlags()
                          << "-std=" << this->confClass->language() << " "
                          << this->pkgClass->headerPaths() << " ";
  } else {
    this->_compileCommand << "cd " << this->_dbgDir << " && "
                          << this->confClass->compilerPath() << " -g "
                          << "-c " << this->confClass->sourceFiles() << " "
                          << this->confClass->compilerDefines() << " "
                          << this->confClass->compilerFlags()
                          << "-std=" << this->confClass->language() << " "
                          << this->pkgClass->headerPaths() << " ";
  }
  system(this->_compileCommand.str().c_str());
  return this->_compileCommand.str();
}

std::string Builder::appLink() {
  std::vector<fs::directory_entry> v;
  std::string temp, tempPath;
  if (this->_buildType == "debug") {
    tempPath = this->_dbgDir;
  } else {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator it = v.begin();
         it != v.end(); ++it) {
      if (fs::extension((*it).path().filename().string()) == ".o" ||
          fs::extension((*it).path().filename().string()) == ".obj") {
        temp += " ";
        temp += (*it).path().filename().string();
      }
    }
  }

  if (this->_buildType == "release") {
    this->_appLinkCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass->compilerPath() << " -o "
                       << this->confClass->binaryName() << " " << temp << " "
                       << this->pkgClass->rlsLibPaths();
  } else {
    this->_appLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass->compilerPath() << " -o "
                       << this->confClass->binaryName() << " " << temp << " "
                       << this->pkgClass->dbgLibPaths();
  }
  system(this->_appLinkCmnd.str().c_str());
  return this->_appLinkCmnd.str();
}

std::string Builder::dylibLink() {
  std::vector<fs::directory_entry> v;
  std::string temp, tempPath;
  if (this->_buildType == "debug") {
    tempPath = this->_dbgDir;
  } else {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator it = v.begin();
         it != v.end(); ++it) {
      if (fs::extension((*it).path().filename().string()) == ".o") {
        temp += " ";
        temp += (*it).path().string();
      }
    }
  }

  std::string dylibArg, dylibExt;
  if (confClass->compilerPath().find("clang") != std::string::npos) {
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  } else {
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (this->_buildType == "release") {
    this->_libLinkCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass->compilerPath() << dylibArg << " -o "
                       << this->confClass->binaryName() << dylibExt << " "
                       << temp << " " << this->pkgClass->rlsLibPaths();
  } else {
    this->_libLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass->compilerPath() << dylibArg << " -o "
                       << this->confClass->binaryName() << dylibExt << " "
                       << temp << " " << this->pkgClass->dbgLibPaths();
  }
  system(this->_libLinkCmnd.str().c_str());
  return this->_libLinkCmnd.str();
}

std::string Builder::archive() {
  std::vector<fs::directory_entry> v;
  std::string temp, tempPath;
  if (this->_buildType == "debug") {
    tempPath = this->_dbgDir;
  } else {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory(static_cast<fs::path>(tempPath))) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator it = v.begin();
         it != v.end(); ++it) {
      if (fs::extension((*it).path().filename().string()) == ".o") {
        temp += " ";
        temp += (*it).path().string();
      }
    }
  }

  if (this->_buildType == "release") {
    this->_archiveCmnd << "cd " << this->_rlsDir << " && "
                       << "ar rcs " << this->confClass->binaryName() << ".a"
                       << " " << temp;
  } else {
    this->_archiveCmnd << "cd " << this->_dbgDir << " && "
                       << "ar rcs " << this->confClass->binaryName() << ".a"
                       << " " << temp;
  }
  system(this->_archiveCmnd.str().c_str());
  return this->_archiveCmnd.str();
}

std::string Builder::getBldCommands() {

  if (this->confClass->binaryType() == "app") {
    return this->compile() + "\n" + this->appLink();
  } else if (this->confClass->binaryType() == "staticLibrary") {
    return this->compile() + "\n" + this->archive();
  } else if (this->confClass->binaryType() == "sharedLibrary") {
    return this->compile() + "\n" + this->dylibLink();
  } else {
    std::cout << "Unknown binary type defined in vcbld.json" << std::endl;
    return "";
  }
}

void Builder::build() {

  if (!fs::exists(this->confClass->outputDirectory()))
    fs::create_directory(this->confClass->outputDirectory());
  if (!fs::exists(this->confClass->outputDirectory().string() + "/" +
                  "release"))
    fs::create_directory(this->_rlsDir);
  if (!fs::exists(this->confClass->outputDirectory().string() + "/" + "debug"))
    fs::create_directory(this->_dbgDir);

  if (this->confClass->binaryType() == "app") {
    this->compile();
    this->appLink();
  } else if (this->confClass->binaryType() == "staticLibrary") {
    this->compile();
    this->archive();
  } else if (this->confClass->binaryType() == "sharedLibrary") {
    this->compile();
    this->dylibLink();
  } else {
    std::cout << "Unknown binary type defined in vcbld.json" << std::endl;
  }
}

Builder::~Builder() {
  delete confClass;
  delete pkgClass;
}
} // namespace vcbld