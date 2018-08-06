#include "init.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

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

namespace vcbld::init {

void setup(const fs::path &vcbldPath) {
  std::string cCompilerPath, cppCompilerPath, cmakePath;
  if (PLATFORM_NAME == "x64-osx") {
    cCompilerPath = "/usr/bin/clang";
    cppCompilerPath = "/usr/bin/clang++";
    cmakePath = "/usr/local/bin/cmake";
  } else if (PLATFORM_NAME == "x64-linux") {
    cCompilerPath = "/usr/bin/gcc";
    cppCompilerPath = "/usr/bin/g++";
    cmakePath = "/usr/local/bin/cmake";
  } else if (PLATFORM_NAME == "x64-windows" || PLATFORM_NAME == "x86-windows") {
    cCompilerPath = "C:/MinGW/bin/gcc";
    cppCompilerPath = "C:/MinGW/bin/g++";
    cmakePath = static_cast<std::string>(getenv("PROGRAMFILES")) + "/CMake/cmake";
  }

  std::string confJsonPath = vcbldPath.string() + "/" + "conf.json";

  if (!fs::exists(confJsonPath)) {

    std::ofstream confOutput(confJsonPath);
    if (confOutput.is_open()) {
      confOutput << std::setw(4) << "{\n\t\"cCompilerPath\" : \""
                 << cCompilerPath << "\",\n\t"
                 << "\"cppCompilerPath\" : \"" << cppCompilerPath << "\",\n\t"
                 << "\"vcpkgDirectory\" : \"" << getenv("HOME") << PATHSEP
                 << "vcpkg"
                 << "\",\n\t"
                 << "\"architecture\" : \"" << PLATFORM_NAME << "\",\n\t"
                 << "\"cmakePath\" : \"" << cmakePath << "\"\n}";
      confOutput.flush();
      confOutput.close();
      std::cout << "conf.json written successfully." << std::endl;
    } else {
    }
  }
}
void init(const std::string &binType) {

  std::string binaryType;
  if (binType == "app") {
    binaryType = "app";
  } else if (binType == "dylib") {
    binaryType = "dynamicLibrary";
  } else if (binType == "statlib") {
    binaryType = "staticLibrary";
  } else {
    binaryType = "app";
  }

  if (!fs::exists("vcbld.json")) {

    std::ofstream vcbldOutput("vcbld.json");
    if (vcbldOutput.is_open()) {
      vcbldOutput << std::setw(4) << "{\n\t\"projectName\" : "
                  << "\"" << fs::current_path().filename().string() << "\",\n\t"
                  << "\"version\" : "
                  << "\"0.1.0\",\n\t"
                  << "\"language\" : "
                  << "\"c++\",\n\t"
                  << "\"standard\" : "
                  << "\"11\",\n\t"
                  << "\"binaryName\" : "
                  << "\"" << fs::current_path().filename().string() << "\",\n\t"
                  << "\"binaryType\" : "
                  << "\"" << binaryType << "\",\n\t"
                  << "\"sourceDirectory\" : "
                  << "\"."
                  << "/"
                  << "src\",\n\t"
                  << "\"outputDirectory\" : "
                  << "\"."
                  << "/"
                  << "bin\",\n\t"
                  << "\"includeDirectory\" : "
                  << "\"."
                  << "/"
                  << "include\",\n\t"
                  << "\"libsDirectory\" : "
                  << "\"."
                  << "/"
                  << "libs\",\n\t"
                  << "\"compilerDefines\" : "
                  << "[],\n\t"
                  << "\"compilerFlags\" : "
                  << "[]\n"
                  << "}";
      vcbldOutput.flush();
      vcbldOutput.close();
      std::cout << "vcbld.json written successfully." << std::endl;
    } else {
    }
  }

  if (!fs::exists("packages.json")) {

    std::ofstream pkgsOutput("packages.json");
    if (pkgsOutput.is_open()) {
      pkgsOutput << std::setw(4) << "{\n\t\"packages\" : []\n}";
      pkgsOutput.flush();
      pkgsOutput.close();
      std::cout << "packages.json written successfully." << std::endl;
    } else {
    }
  }
}
} // namespace vcbld::init