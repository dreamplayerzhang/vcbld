#include "init.h"

#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#if defined(_WIN32)
#define PLATFORM_NAME "x86-windows"
#define PATHSEP "\\"
#elif defined(_WIN64)
#define PLATFORM_NAME "x64-windows"
#define PATHSEP "\\"
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

namespace fs = std::experimental::filesystem;

namespace vcbld
{
namespace init
{
void setup(const fs::path &vcbldPath)
{
  std::string cCompilerPath, cppCompilerPath, cmakePath, makePath;
  std::string cmakeDir = vcbldPath.string() + "/downloads/tools";
  if (PLATFORM_NAME == "x64-osx")
  {
    cCompilerPath = "/usr/bin/clang";
    cppCompilerPath = "/usr/bin/clang++";
    if (findCmake(cmakeDir) == "")
    {
      cmakePath = "/usr/local/bin/cmake";
    }
    else
    {
      cmakePath = findCmake(cmakeDir);
    }
    makePath = "/usr/bin/make";
  }
  else if (PLATFORM_NAME == "x64-linux")
  {
    cCompilerPath = "/usr/bin/gcc";
    cppCompilerPath = "/usr/bin/g++";
    if (findCmake(cmakeDir) == "")
    {
      cmakePath = "/usr/local/bin/cmake";
    }
    else
    {
      cmakePath = findCmake(cmakeDir);
    }
    // cmakePath = "/usr/bin/cmake";
    makePath = "/usr/bin/make";
  }
  else if (PLATFORM_NAME == "x64-windows" || PLATFORM_NAME == "x86-windows")
  {
    cCompilerPath = "C:/MinGW/bin/gcc";
    cppCompilerPath = "C:/MinGW/bin/g++";
    if (findCmake(cmakeDir) == "")
    {
      cmakePath = "/usr/local/bin/cmake";
    }
    else
    {
      cmakePath = findCmake(cmakeDir);
    }
    // cmakePath = static_cast<std::string>(getenv("PROGRAMFILES")) +
    // "/CMake/cmake.exe";
    makePath = "C:/MinGW/bin/mingw32-make.exe";
  }

  if (!fs::exists("conf.json"))
  {

    std::ofstream confOutput("conf.json");
    if (confOutput.is_open())
    {
      confOutput << std::setw(4) << "{\n\t\"cCompilerPath\" : \""
                 << cCompilerPath << "\",\n\t"
                 << "\"cppCompilerPath\" : \"" << cppCompilerPath << "\",\n\t"
                 << "\"vcpkgDirectory\" : \""
                 << static_cast<std::string> (fs::absolute(vcbldPath)) << "\",\n\t"
                 << "\"architecture\" : \"" << PLATFORM_NAME << "\",\n\t"
                 << "\"cmakePath\" : \"" << cmakePath << "\",\n\t"
                 << "\"makePath\" : \"" << makePath << "\"\n}";
      confOutput.flush();
      confOutput.close();
      std::cout << "conf.json written successfully." << std::endl;
    }
    else
    {
    }
  }
}

void init(const std::string &binType)
{
  std::string binaryType;
  if (binType == "app")
  {
    binaryType = "app";
  }
  else if (binType == "dylib")
  {
    binaryType = "dynamicLibrary";
  }
  else if (binType == "statlib")
  {
    binaryType = "staticLibrary";
  }
  else
  {
    binaryType = "app";
  }

  if (!fs::exists("vcbld.json"))
  {

    std::ofstream vcbldOutput("vcbld.json");
    if (vcbldOutput.is_open())
    {
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
                  << "\"src\",\n\t"
                  << "\"outputDirectory\" : "
                  << "\"bin\",\n\t"
                  << "\"includeDirectory\" : "
                  << "\"include\",\n\t"
                  << "\"libsDirectory\" : "
                  << "\"libs\",\n\t"
                  << "\"compilerDefines\" : "
                  << "[],\n\t"
                  << "\"compilerFlags\" : "
                  << "[]\n"
                  << "}";
      vcbldOutput.flush();
      vcbldOutput.close();
      std::cout << "vcbld.json written successfully." << std::endl;
    }
    else
    {
      std::cout << "vcbld.json exists." << std::endl;
    }
  }

  if (!fs::exists("package.json"))
  {

    std::ofstream pkgsOutput("package.json");
    if (pkgsOutput.is_open())
    {
      pkgsOutput << std::setw(4) << "{\n\t\"packages\" : []\n}";
      pkgsOutput.flush();
      pkgsOutput.close();
      std::cout << "package.json written successfully." << std::endl;
    }
    else
    {
      std::cout << "package.json exists." << std::endl;
    }
  }
}

std::string findCmake(const std::string &dir)
{
  std::string temp;
  std::string fileName = "bin";
  const fs::recursive_directory_iterator end;
  const auto it = std::find_if(fs::recursive_directory_iterator(dir), end,
                               [&fileName](const fs::directory_entry &e) {
                                 return e.path().filename() == fileName;
                               });
  if (it == end)
  {
    temp = "";
    return temp;
  }
  else
  {
    temp = fs::absolute(it->path()).string();
    std::string cmakeName;
    if (PLATFORM_NAME == "x86-windows" || PLATFORM_NAME == "x64-windows")
    {
      cmakeName = "cmake.exe";
    }
    else
    {
      cmakeName = "cmake";
    }
    return temp + "/" + cmakeName;
  }
}
} // namespace init
} // namespace vcbld
