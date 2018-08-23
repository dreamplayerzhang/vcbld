#include "builder.h"

#include <algorithm>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <iostream>
#include <iterator>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace vcbld {

Builder::Builder(const std::string &buildType)
    : _buildType(buildType), PrepClass() {
  if (!fs::exists("vcbld.json")) {
    std::cout << "Build configuration not found!" << std::endl;
    std::exit(1);
  }

  this->_dbgDir = this->outputDirectory() + "/" + "debug";
  this->_rlsDir = this->outputDirectory() + "/" + "release";

  if (this->_buildType == "debug") {
    this->_buildDir = this->_dbgDir;
  } else {
    this->_buildDir = this->_rlsDir;
  }
}

void Builder::compile() {

  if (this->_buildType == "release") {
    this->_compileCommand << "cd " << this->_rlsDir << " && \""
                          << this->compilerPath() << "\" "
                          << this->headerPaths() << " " << this->compilerFlags()
                          << " " << this->compilerDefines() << " "
                          << "-std=" << this->language() << this->standard()
                          << " -c " << this->sourceFiles();
  } else {
    this->_compileCommand << "cd " << this->_dbgDir << " && \""
                          << this->compilerPath() << "\" "
                          << this->headerPaths() << " " << this->compilerFlags()
                          << " " << this->compilerDefines() << " -g "
                          << "-std=" << this->language() << this->standard()
                          << " -c " << this->sourceFiles();
  }
  int systemRet = system(this->_compileCommand.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while compiling." << std::endl;
  }
}

void Builder::appLink() {
  if (this->_buildType == "release") {
    this->_appLinkCmnd << "cd " << this->_rlsDir << " && \""
                       << this->compilerPath() << "\" -o " << this->binaryName()
                       << " " << this->objPath(this->_buildDir) << " "
                       << this->rlsLibPaths() << " " << this->linkerFlags();
  } else {
    this->_appLinkCmnd << "cd " << this->_dbgDir << " && \""
                       << this->compilerPath() << "\" -o " << this->binaryName()
                       << " " << this->objPath(this->_buildDir) << " "
                       << this->dbgLibPaths() << " " << this->linkerFlags();
  }
  int systemRet = system(this->_appLinkCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while linking." << std::endl;
  }
}

void Builder::dylibLink() {
  std::string dylibArg, dylibExt;
  if (this->compilerPath().find("clang") != std::string::npos) {
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  } else {
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (this->_buildType == "release") {
    this->_libLinkCmnd << "cd " << this->_rlsDir << " && \""
                       << this->compilerPath() << "\"" << dylibArg << " -o "
                       << this->binaryName() << dylibExt << " "
                       << this->objPath(this->_buildDir) << " "
                       << this->rlsLibPaths() << " " << this->linkerFlags();
  } else {
    this->_libLinkCmnd << "cd " << this->_dbgDir << " && \""
                       << this->compilerPath() << "\"" << dylibArg << " -o "
                       << this->binaryName() << dylibExt << " "
                       << this->objPath(this->_buildDir) << " "
                       << this->dbgLibPaths() << " " << this->linkerFlags();
  }
  int systemRet = system(this->_libLinkCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while linking." << std::endl;
  }
}

void Builder::archive() {
  if (this->_buildType == "release") {
    this->_archiveCmnd << "cd " << this->_rlsDir << " && \""
                       << this->archiverPath() << "\" rcs "
                       << this->binaryName() << ".a"
                       << " " << this->objPath(this->_buildDir);
  } else {
    this->_archiveCmnd << "cd " << this->_dbgDir << " && \""
                       << this->archiverPath() << "\" rcs "
                       << this->binaryName() << ".a"
                       << " " << this->objPath(this->_buildDir);
  }
  int systemRet = system(this->_archiveCmnd.str().c_str());
  if (systemRet == -1) {
    std::cout << "An error occured while archiving." << std::endl;
  }
}

std::string Builder::getBldCommands() {
  if (this->binaryType() == "app") {
    return this->_compileCommand.str() + "\n" + this->_appLinkCmnd.str();
  } else if (this->binaryType() == "staticLibrary") {
    return this->_compileCommand.str() + "\n" + this->_archiveCmnd.str();
  } else if (this->binaryType() == "dynamicLibrary") {
    return this->_compileCommand.str() + "\n" + this->_libLinkCmnd.str();
  } else {
    std::cout << "Unknown binary type defined in vcbld.json." << std::endl;
    return "";
  }
}

void Builder::build() {

  if (!fs::exists(this->outputDirectory()))
    fs::create_directory(this->outputDirectory());
  if (!fs::exists(this->outputDirectory() + "/" + "release"))
    fs::create_directory(this->_rlsDir);
  if (!fs::exists(this->outputDirectory() + "/" + "debug"))
    fs::create_directory(this->_dbgDir);

  if (this->binaryType() == "app") {
    try {
      std::cout << "Compiling in " << this->_buildType << "...\n";
      this->compile();
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Linking...\n";
      this->appLink();
    } catch (const std::exception &e) {
      std::cout << "Linking failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      this->copy();
    } catch (const std::exception &e) {
      std::cout << "Libraries exist in output directory." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else if (this->binaryType() == "staticLibrary") {
    try {
      std::cout << "Compiling in " << this->_buildType << "...\n";
      this->compile();
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Archiving...\n";
      this->archive();
    } catch (const std::exception &e) {
      std::cout << "Archiving failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
  } else if (this->binaryType() == "dynamicLibrary") {
    try {
      std::cout << "Compiling in " << this->_buildType << "...\n";
      this->compile();
    } catch (const std::exception &e) {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try {
      std::cout << "Linking...\n";
      this->dylibLink();
    } catch (const std::exception &e) {
      std::cout << "Linking failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
    try {
      this->copy();
    } catch (const std::exception &e) {
      std::cout << "Libraries exist in output directory." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  } else {
    std::cout << "Unknown binary type defined in vcbld.json." << std::endl;
  }
}

void Builder::copy() {
  std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                           this->architecture() + "/" + "debug/lib";
  std::string rlsLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                           this->architecture() + "/" + "lib";
  std::string fullName;
  if (this->libDirectory() != "") {
    std::string localDbgPath = this->libDirectory() + "/" + "debug";
    std::string localRlsPath = this->libDirectory() + "/" + "release";
    if (this->_buildType == "debug") {
      for (std::vector<std::string>::iterator it =
               this->dbgLocalLibNames().begin();
           it != this->dbgLocalLibNames().end(); ++it) {
        fullName = localDbgPath + "/" + (*it);
        if (fs::exists(fullName)) {
          if ((*it).find(".a") == std::string::npos &&
              (*it).find(".lib") == std::string::npos) {
            if (!fs::exists(this->_dbgDir + "/" + (*it))) {
              fs::copy(fullName, this->_dbgDir);
            }
          }
        }
      }
    } else {
      for (std::vector<std::string>::iterator it =
               this->rlsLocalLibNames().begin();
           it != this->rlsLocalLibNames().end(); ++it) {
        fullName = localRlsPath + "/" + (*it);
        if (fs::exists(fullName)) {
          if ((*it).find(".a") == std::string::npos &&
              (*it).find(".lib") == std::string::npos) {
            if (!fs::exists(this->_rlsDir + "/" + (*it))) {
              fs::copy(fullName, this->_rlsDir);
            }
          }
        }
      }
    }
  }
  for (std::vector<std::string>::iterator it = this->fullLibNames().begin();
       it != this->fullLibNames().end(); ++it) {
    if (this->_buildType == "debug") {
      fullName = dbgLibPath + "/" + (*it);
    } else {
      fullName = rlsLibPath + "/" + (*it);
    }
    if (fs::exists(fullName)) {
      if (this->_buildType == "debug") {
        if ((*it).find(".a") == std::string::npos &&
            (*it).find(".lib") == std::string::npos) {
          if (!fs::exists(this->_dbgDir + "/" + (*it))) {
            fs::copy(fullName, this->_dbgDir);
          }
        }
      } else {
        if ((*it).find(".a") == std::string::npos &&
            (*it).find(".lib") == std::string::npos) {
          if (!fs::exists(this->_rlsDir + "/" + (*it))) {
            fs::copy(fullName, this->_rlsDir);
          }
        }
      }
    }
  }
}

} // namespace vcbld
