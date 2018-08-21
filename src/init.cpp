#include "init.h"

#include <algorithm>
#include <errno.h>
#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <typeinfo>
#include <vector>

#include "conf.h"

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
  std::string PATH = getenv("PATH");
  std::string cmakeDir = vcbldPath.string() + "/downloads/tools";
  std::string brewLLVM = "/usr/local/opt/llvm/bin";
  std::vector<std::string> paths, cCompilers, cppCompilers, cmakePaths,
      makePaths, archiverPaths;
  std::string cCompilerPath, cppCompilerPath, cmakePath, makePath, archiverPath;

  findPathDirs(PATH, paths);
  paths.emplace_back(findCmake(cmakeDir));

  if (fs::exists(brewLLVM))
  {
    paths.emplace_back(brewLLVM);
  }

  if (PLATFORM_NAME == "x64-windows" || PLATFORM_NAME == "x86-windows")
  {
    if (fs::is_directory("C:/"))
    {
      std::vector<fs::directory_entry> dirEntry;
      std::copy(fs::directory_iterator("C:/"), fs::directory_iterator(),
                back_inserter(dirEntry));
      for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
           it != dirEntry.end(); ++it)
      {
        if ((*it).path().string().find("MinGW") != std::string::npos)
        {
          std::string binDir = (*it).path().string() + "/bin";
          paths.emplace_back(binDir);
        }
      }
    }
    try
    {
      std::string progFiles = getenv("PROGRAMFILES");
      if (fs::exists(progFiles))
      {
        std::vector<fs::directory_entry> dirEntry2;
        std::copy(fs::directory_iterator(progFiles), fs::directory_iterator(),
                  back_inserter(dirEntry2));
        for (std::vector<fs::directory_entry>::iterator it = dirEntry2.begin();
             it != dirEntry2.end(); ++it)
        {
          if ((*it).path().string().find("MinGW") != std::string::npos)
          {
            std::string binDir = (*it).path().string() + "/bin";
            paths.emplace_back(binDir);
          }
          if ((*it).path().string().find("cmake") != std::string::npos)
          {
            std::string binDir = (*it).path().string() + "/bin";
            paths.emplace_back(binDir);
          }
        }
      }
    }
    catch (...)
    {
      // fail quietly
    }

    try
    {
      std::string progFiles = getenv("MINGW_HOME");
      if (fs::is_directory(progFiles))
      {
        std::vector<fs::directory_entry> dirEntry;
        std::copy(fs::directory_iterator(progFiles), fs::directory_iterator(),
                  back_inserter(dirEntry));
        std::sort(dirEntry.begin(), dirEntry.end());
        dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                       dirEntry.end());
        for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
             it != dirEntry.end(); ++it)
        {
          if ((*it).path().string().find("MinGW") != std::string::npos)
          {
            std::string binDir = (*it).path().string() + "/bin";
            paths.emplace_back(binDir);
          }
          if ((*it).path().string().find("cmake") != std::string::npos)
          {
            std::string binDir = (*it).path().string() + "/bin";
            paths.emplace_back(binDir);
          }
        }
      }
    }
    catch (...)
    {
      // fail quietly
    }
  }

  std::sort(paths.begin(), paths.end());
  paths.erase(std::unique(paths.begin(), paths.end()), paths.end());

  for (std::vector<std::string>::iterator it = paths.begin(); it != paths.end();
       ++it)
  {
    replaceHome(*it);

    if (fs::exists((*it) + "/gcc"))
    {
      cCompilers.emplace_back((*it) + "/gcc");
    }
    if (fs::exists((*it) + "/gcc.exe"))
    {
      cCompilers.emplace_back((*it) + "/gcc.exe");
    }
    if (fs::exists((*it) + "/gcc-8"))
    {
      cCompilers.emplace_back((*it) + "/gcc-8");
    }
    if (fs::exists((*it) + "/gcc-7"))
    {
      cCompilers.emplace_back((*it) + "/gcc-7");
    }
    if (fs::exists((*it) + "/gcc-6"))
    {
      cCompilers.emplace_back((*it) + "/gcc-6");
    }
    if (fs::exists((*it) + "/gcc-5"))
    {
      cCompilers.emplace_back((*it) + "/gcc-6");
    }
    if (fs::exists((*it) + "/clang"))
    {
      cCompilers.emplace_back((*it) + "/clang");
    }
    if (fs::exists((*it) + "/clang.exe"))
    {
      cCompilers.emplace_back((*it) + "/clang.exe");
    }
    if (fs::exists((*it) + "/clang-6.0"))
    {
      cCompilers.emplace_back((*it) + "/clang-6.0");
    }
    if (fs::exists((*it) + "/clang-5.0"))
    {
      cCompilers.emplace_back((*it) + "/clang-5.0");
    }
    if (fs::exists((*it) + "/clang-4.0"))
    {
      cCompilers.emplace_back((*it) + "/clang-4.0");
    }
    if (fs::exists((*it) + "/g++"))
    {
      cppCompilers.emplace_back((*it) + "/g++");
    }
    if (fs::exists((*it) + "/g++.exe"))
    {
      cppCompilers.emplace_back((*it) + "/g++.exe");
    }
    if (fs::exists((*it) + "/g++-8"))
    {
      cppCompilers.emplace_back((*it) + "/g++-8");
    }
    if (fs::exists((*it) + "/g++-7"))
    {
      cppCompilers.emplace_back((*it) + "/g++-7");
    }
    if (fs::exists((*it) + "/g++-6"))
    {
      cppCompilers.emplace_back((*it) + "/g++-6");
    }
    if (fs::exists((*it) + "/clang++"))
    {
      cppCompilers.emplace_back((*it) + "/clang++");
    }
    if (fs::exists((*it) + "/cmake"))
    {
      cmakePaths.emplace_back((*it) + "/cmake");
    }
    if (fs::exists((*it) + "/make"))
    {
      makePaths.emplace_back((*it) + "/make");
    }
    if (fs::exists((*it) + "/make.exe"))
    {
      makePaths.emplace_back((*it) + "/make.exe");
    }
    if (fs::exists((*it) + "/mingw32-make.exe"))
    {
      makePaths.emplace_back((*it) + "/mingw32-make.exe");
    }
    if (fs::exists((*it) + "/ar"))
    {
      archiverPaths.emplace_back((*it) + "/ar");
    }
    if (fs::exists((*it) + "/ar.exe"))
    {
      archiverPaths.emplace_back((*it) + "/ar.exe");
    }
  }

  cCompilerPath = cCompiler(cCompilers);
  cppCompilerPath = cppCompiler(cppCompilers);
  cmakePath = cmake(cmakePaths);
  makePath = make(makePaths);
  archiverPath = archiver(archiverPaths);

  if (fs::exists("conf.json"))
  {
    fs::remove("conf.json");
  }
  
  if (!fs::exists("conf.json"))
  {
    try
    {
      std::ofstream confOutput("conf.json");
      if (confOutput.is_open())
      {
        confOutput << std::setw(4) << "{\n\t\"cCompilerPath\" : \""
                   << cCompilerPath << "\",\n\t"
                   << "\"cppCompilerPath\" : \"" << cppCompilerPath << "\",\n\t"
                   << "\"vcpkgDirectory\" : \""
                   << static_cast<std::string>(fs::canonical(vcbldPath))
                   << "\",\n\t"
                   << "\"architecture\" : \"" << PLATFORM_NAME << "\",\n\t"
                   << "\"cmakePath\" : \"" << cmakePath << "\",\n\t"
                   << "\"makePath\" : \"" << makePath << "\",\n\t"
                   << "\"archiverPath\" : \"" << archiverPath << "\"\n}";
        confOutput.flush();
        confOutput.close();
        std::cout << "conf.json written successfully." << std::endl;
      }
      else
      {
        std::cout << "Error opening conf.json." << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }
  else
  {
    std::cout << "conf.json was found in the directory." << std::endl;
  }

  if (!fs::exists("package.json"))
  {
    try
    {
      std::ofstream pkgsOutput("package.json");
      if (pkgsOutput.is_open())
      {
        pkgsOutput << std::setw(4) << "{\n\t\"packages\" : []\n}";
        pkgsOutput.flush();
        pkgsOutput.close();
        std::cout << "package.json written successfully." << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }

  if (fs::exists("vcbld.json"))
  {
    ConfClass confClass;
    if (!fs::exists(confClass.outputDirectory()))
    {
      fs::create_directory(confClass.outputDirectory());
      fs::create_directory(confClass.outputDirectory().string() + "/debug");
      fs::create_directory(confClass.outputDirectory().string() + "/release");
      std::cout << "output directory created successfully." << std::endl;
    }

    if (confClass.includeDirectory() != "")
    {
      if (!fs::exists(confClass.includeDirectory()))
      {
        fs::create_directory(confClass.includeDirectory());
        std::cout << "include directory created successfully." << std::endl;
      }
    }

    if (confClass.includeDirectory() != "")
    {
      if (!fs::exists(confClass.libDirectory()) &&
          confClass.libDirectory() != "")
      {
        fs::create_directory(confClass.libDirectory());
        fs::create_directory(confClass.libDirectory().string() + "/debug");
        fs::create_directory(confClass.libDirectory().string() + "/release");
        std::cout << "lib directory created successfully." << std::endl;
      }
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
    try
    {
      std::ofstream vcbldOutput("vcbld.json");
      if (vcbldOutput.is_open())
      {
        vcbldOutput << std::setw(4) << "{\n\t\"projectName\" : "
                    << "\"" << fs::current_path().filename().string()
                    << "\",\n\t"
                    << "\"version\" : "
                    << "\"0.1.0\",\n\t"
                    << "\"language\" : "
                    << "\"c++\",\n\t"
                    << "\"standard\" : "
                    << "\"11\",\n\t"
                    << "\"binaryName\" : "
                    << "\"" << fs::current_path().filename().string()
                    << "\",\n\t"
                    << "\"binaryType\" : "
                    << "\"" << binaryType << "\",\n\t"
                    << "\"sourceDirectory\" : "
                    << "\"src\",\n\t"
                    << "\"outputDirectory\" : "
                    << "\"bin\",\n\t"
                    << "\"includeDirectory\" : "
                    << "\"include\",\n\t"
                    << "\"libDirectory\" : "
                    << "\"lib\",\n\t"
                    << "\"compilerDefines\" : "
                    << "[],\n\t"
                    << "\"compilerFlags\" : "
                    << "[],\n\t"
                    << "\"linkerFlags\" : "
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
    catch (const std::exception &e)
    {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }

  if (!fs::exists("package.json"))
  {
    try
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
    catch (const std::exception &e)
    {
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }
}

void findPathDirs(std::string &PATH, std::vector<std::string> &dirs)
{
  std::string rem;
  size_t foundSep;
  std::string sep;
  if (PATH.find(";") != std::string::npos)
  {
    sep = ";";
    foundSep = PATH.find_first_of(sep);
  }
  else
  {
    sep = ":";
    foundSep = PATH.find_first_of(sep);
  }
  dirs.emplace_back(PATH.substr(0, foundSep));
  rem = PATH.substr(foundSep + 1, PATH.length());
  if (rem.find_last_of(sep) != std::string::npos)
  {
    findPathDirs(rem, dirs);
  }
}

void replaceHome(std::string &path)
{
  std::string home = std::getenv("HOME");

  if (path[0] == '~')
  {
    path.replace(0, 1, home);
  }
}

std::string findCmake(const std::string &dir)
{
  std::string temp;
  std::string fileName = "bin";
  const fs::recursive_directory_iterator end;
  try
  {
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
      temp = fs::canonical(it->path()).string();
    }
  }
  catch (const std::exception &exception)
  {
    std::cout << "Couldn't find a cmake executable in the current directory."
              << std::endl;
  }
  return temp;
}

std::string cCompiler(const std::vector<std::string> &cCompilers)
{
  if (cCompilers.size() == 0)
  {
    std::cout << "vcbld couldn't locate a c compiler." << std::endl;
    return "";
  }
  else
  {
    std::cout << "list of available c compilers:" << std::endl;
    int i = 1;
    for (std::vector<std::string>::const_iterator it = cCompilers.begin();
         it != cCompilers.end(); ++it)
    {
      std::cout << i << ") " << *it << std::endl;
      i++;
    }

    std::cout << "Please choose your project's c compiler." << std::endl;
    int entry;
    std::cin >> entry;
    if (entry <= cCompilers.size() && entry != 0 &&
        typeid(entry) == typeid(int))
    {
      std::cout << "You have chosen:" << cCompilers[entry - 1] << std::endl;
      return cCompilers[entry - 1];
    }
    else
    {
      std::cout << "Error!" << std::endl;
      cCompiler(cCompilers);
      return "";
    }
  }
}

std::string cppCompiler(const std::vector<std::string> &cppCompilers)
{
  if (cppCompilers.size() == 0)
  {
    std::cout << "vcbld couldn't locate a c++ compiler." << std::endl;
    return "";
  }
  else
  {
    std::cout << "list of available c++ compilers:" << std::endl;
    int j = 1;
    for (std::vector<std::string>::const_iterator it = cppCompilers.begin();
         it != cppCompilers.end(); ++it)
    {
      std::cout << j << ") " << *it << std::endl;
      j++;
    }
    int entry;
    std::cin >> entry;
    if (entry <= cppCompilers.size() && entry != 0 &&
        typeid(entry) == typeid(int))
    {
      std::cout << "You have chosen:" << cppCompilers[entry - 1] << std::endl;
      return cppCompilers[entry - 1];
    }
    else
    {
      std::cout << "Error!" << std::endl;
      cppCompiler(cppCompilers);
      return "";
    }
  }
}

std::string cmake(const std::vector<std::string> &cmakes)
{
  if (cmakes.size() == 0)
  {
    std::cout << "vcbld couldn't locate a cmake executable." << std::endl;
    return "";
  }
  else
  {
    std::cout << "list of available cmake executables:" << std::endl;
    int cmake_it = 1;
    for (std::vector<std::string>::const_iterator it = cmakes.begin();
         it != cmakes.end(); ++it)
    {
      std::cout << cmake_it << ") " << *it << std::endl;
      cmake_it++;
    }
    int entry;
    std::cin >> entry;
    if (entry <= cmakes.size() && entry != 0 && typeid(entry) == typeid(int))
    {
      std::cout << "You have chosen:" << cmakes[entry - 1] << std::endl;
      return cmakes[entry - 1];
    }
    else
    {
      std::cout << "Error!" << std::endl;
      cmake(cmakes);
      return "";
    }
  }
}

std::string make(const std::vector<std::string> &makes)
{
  if (makes.size() == 0)
  {
    std::cout << "vcbld couldn't locate a make executable." << std::endl;
    return "";
  }
  else
  {
    std::cout << "list of available make executables:" << std::endl;
    int cmake_it = 1;
    for (std::vector<std::string>::const_iterator it = makes.begin();
         it != makes.end(); ++it)
    {
      std::cout << cmake_it << ") " << *it << std::endl;
      cmake_it++;
    }
    int entry;
    std::cin >> entry;
    if (entry <= makes.size() && entry != 0 && typeid(entry) == typeid(int))
    {
      std::cout << "You have chosen:" << makes[entry - 1] << std::endl;
      return makes[entry - 1];
    }
    else
    {
      std::cout << "Error!" << std::endl;
      make(makes);
      return "";
    }
  }
}
std::string archiver(const std::vector<std::string> &archivers)
{
  if (archivers.size() == 0)
  {
    std::cout << "vcbld couldn't locate an archiver." << std::endl;
    return "";
  }
  else
  {
    std::cout << "list of available archivers:" << std::endl;
    int cmake_it = 1;
    for (std::vector<std::string>::const_iterator it = archivers.begin();
         it != archivers.end(); ++it)
    {
      std::cout << cmake_it << ") " << *it << std::endl;
      cmake_it++;
    }
    int entry;
    std::cin >> entry;
    if (entry <= archivers.size() && entry != 0 &&
        typeid(entry) == typeid(int))
    {
      std::cout << "You have chosen:" << archivers[entry - 1] << std::endl;
      return archivers[entry - 1];
    }
    else
    {
      std::cout << "Error!" << std::endl;
      archiver(archivers);
      return "";
    }
  }
}

} // namespace init
} // namespace vcbld
