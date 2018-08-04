#include "builder.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <iterator>
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

namespace fs = boost::filesystem;

namespace vcbld {

Builder::Builder(const char *buildType) {

  this->_buildType = buildType;
  if (!fs::exists("vcbld.json")) {
    std::cout << "Build configuration not found!" << std::endl;
    std::exit(1);
  }

  this->_dbgDir =
      this->confClass.outputDirectory().string() + PATHSEP + "debug";
  this->_rlsDir =
      this->confClass.outputDirectory().string() + PATHSEP + "release";
}

std::string Builder::compile() {

  if (strcmp(this->_buildType, "release") == 0) {
    this->_compileCommand << "cd " << this->_rlsDir << " && "
                          << this->confClass.compilerPath() << " -c "
                          << this->confClass.sourceFiles() << " "
                          << this->confClass.compilerDefines() << " "
                          << this->confClass.compilerFlags()
                          << "-std=" << this->confClass.language() << " "
                          << this->pkgClass.headerPaths() << " ";
  } else {
    this->_compileCommand << "cd " << this->_dbgDir << " && "
                          << this->confClass.compilerPath() << " -g "
                          << "-c " << this->confClass.sourceFiles() << " "
                          << this->confClass.compilerDefines() << " "
                          << this->confClass.compilerFlags()
                          << "-std=" << this->confClass.language() << " "
                          << this->pkgClass.headerPaths() << " ";
  }
  return _compileCommand.str();
}

std::string Builder::appLink() {
  std::vector<fs::directory_entry> v;
  std::string temp, tempPath;
  if (strcmp(this->_buildType, "debug") == 0) {
    tempPath = this->_dbgDir;
  } else {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory((fs::path)tempPath)) {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(v));

    for (std::vector<fs::directory_entry>::iterator it = v.begin();
         it != v.end(); ++it) {
      if (fs::extension((*it).path().filename().string()) == ".o") {
        temp += " ";
        temp += (*it).path().filename().string();
      }
    }
  }

  if (strcmp(this->_buildType, "release") == 0) {
    this->_appLinkCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass.compilerPath() << " -o "
                       << this->confClass.binaryName() << " " << temp << " "
                       << this->pkgClass.rlsLibPaths();
  } else {
    this->_appLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass.compilerPath() << " -o "
                       << this->confClass.binaryName() << " " << temp << " "
                       << this->pkgClass.dbgLibPaths();
  }
  return this->_appLinkCmnd.str();
}

std::string Builder::dylibLink() {
  std::vector<fs::directory_entry> v;
  std::string temp, tempPath;
  if (strcmp(this->_buildType, "debug") == 0) {
    tempPath = this->_dbgDir;
  } else {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory((fs::path)tempPath)) {
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
  if (confClass.compilerPath().find("clang") != std::string::npos) {
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  } else {
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (strcmp(this->_buildType, "release") == 0) {
    this->_libLinkCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass.compilerPath() << dylibArg << " -o "
                       << this->confClass.binaryName() << dylibExt << " "
                       << temp << " " << this->pkgClass.rlsLibPaths();
  } else {
    this->_libLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass.compilerPath() << dylibArg << " -o "
                       << this->confClass.binaryName() << dylibExt << " "
                       << temp << " " << this->pkgClass.dbgLibPaths();
  }
  return this->_libLinkCmnd.str();
}

std::string Builder::archive() {
  std::vector<fs::directory_entry> v;
  std::string temp, tempPath;
  if (strcmp(this->_buildType, "debug") == 0) {
    tempPath = this->_dbgDir;
  } else {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory((fs::path)tempPath)) {
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

  if (strcmp(this->_buildType, "release") == 0) {
    this->_archiveCmnd << "cd " << this->_rlsDir << " && "
                       << "ar rcs " << this->confClass.binaryName() << ".a"
                       << " " << temp;
  } else {
    this->_archiveCmnd << "cd " << this->_dbgDir << " && "
                       << "ar rcs " << this->confClass.binaryName() << ".a"
                       << " " << temp;
  }
  return this->_archiveCmnd.str();
}

std::string Builder::getBldCommands() {

  if (this->confClass.binaryType() == "app") {
    return this->compile() + "\n" + this->appLink();
  } else if (this->confClass.binaryType() == "staticLibrary") {
    return this->compile() + "\n" + this->archive();
  } else if (this->confClass.binaryType() == "sharedLibrary") {
    return this->compile() + "\n" + this->dylibLink();
  } else {
    std::cout << "Unknown binary type defined in vcbld.json" << std::endl;
    return "";
  }
}

void Builder::build() {

  if (!fs::exists(this->confClass.outputDirectory()))
    fs::create_directory(this->confClass.outputDirectory());
  if (!fs::exists(this->confClass.outputDirectory().string() + PATHSEP +
                  "release"))
    fs::create_directory(this->_rlsDir);
  if (!fs::exists(this->confClass.outputDirectory().string() + PATHSEP +
                  "debug"))
    fs::create_directory(this->_dbgDir);

  if (this->confClass.binaryType() == "app") {
    system(this->compile().c_str());
    system(this->appLink().c_str());
  } else if (this->confClass.binaryType() == "staticLibrary") {
    system(this->compile().c_str());
    system(this->archive().c_str());
  } else if (this->confClass.binaryType() == "sharedLibrary") {
    system(this->compile().c_str());
    system(this->dylibLink().c_str());
  } else {
    std::cout << "Unknown binary type defined in vcbld.json" << std::endl;
  }
}

} // namespace vcbld