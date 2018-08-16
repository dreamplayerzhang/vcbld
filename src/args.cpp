#include "args.h"

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#include "builder.h"
#include "conf.h"
#include "gen.h"
#include "init.h"

namespace fs = std::experimental::filesystem;

namespace vcbld
{
namespace args
{

void New(const std::string &binType)
{

  if (!fs::exists("src"))
  {
    fs::create_directory("src");
    std::cout << "src directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "src directroy exists." << std::endl;
  }

  if (!fs::exists("include"))
  {
    fs::create_directory("include");
    std::cout << "include directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "include directroy exists." << std::endl;
  }

  if (!fs::exists("libs"))
  {
    fs::create_directory("libs");
    std::cout << "libs directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "libs directroy exists." << std::endl;
  }

  if (!fs::exists("./libs/debug"))
  {
    fs::create_directory("./libs/debug");
    std::cout << "libs/debug directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "libs/debug directroy exists." << std::endl;
  }

  if (!fs::exists("./libs/release"))
  {
    fs::create_directory("./libs/release");
    std::cout << "libs/release directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "libs/release directroy exists." << std::endl;
  }

  if (!fs::exists("bin"))
  {
    fs::create_directory("bin");
    std::cout << "bin directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "bin directroy exists." << std::endl;
  }

  if (!fs::exists("./bin/debug"))
  {
    fs::create_directory("./bin/debug");
    std::cout << "bin/debug directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "bin/debug directroy exists." << std::endl;
  }

  if (!fs::exists("./bin/release"))
  {
    fs::create_directory("bin/release");
    std::cout << "bin/release directory created successfully." << std::endl;
  }
  else
  {
    std::cout << "bin/release directroy exists." << std::endl;
  }

  if (!fs::exists("./src/main.cpp") && binType == "app")
  {
    std::ofstream ofs("./src/main.cpp");

    if (ofs.is_open())
    {
      ofs << "#include <iostream>\n\n"
          << "int main(int argc, char *argv[])\n{\n\tstd::cout  << \"Hello "
             "World\"  << std::endl; \n\treturn 0;\n}"
          << std::endl;
      ofs.flush();
      ofs.close();
      std::cout << "main.cpp written successfully." << std::endl;
    }
    else
    {
      std::cerr << "Failed to open file : " << errno << std::endl;
    }
  }
  else
  {
    std::cout << "main.cpp already exists." << std::endl;
  }
  init::setup();
  init::init(binType);
}

void build(const std::string &buildType)
{
  Builder builder(buildType);
  builder.build();
}

void clean()
{
  try
  {
    ConfClass confClass;
    std::string command = "rm -rf " + confClass.outputDirectory().string() +
                          "/" + "debug" + "/" + "**";
    int systemRet = system(command.c_str());
    if (systemRet == -1)
    {
      std::cout << "An error occured while deleting output." << std::endl;
    }
    command = "rm -rf " + confClass.outputDirectory().string() + "/" +
              "release" + "/" + "**";
    int systemRet2 = system(command.c_str());
    if (systemRet2 == -1)
    {
      std::cout << "An error occured while deleting output." << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "vcbld.json not found!" << std::endl;
  }
}

void run(const std::string &buildType)
{
  std::ostringstream command;
  if (buildType == "debug")
  {
    try
    {
      ConfClass confClass;
      command << "./" << confClass.outputDirectory() << "/debug/"
              << confClass.binaryName();
      int systemRet = system(command.str().c_str());
      if (systemRet == -1)
      {
        std::cout << "An error occured while running the binary." << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cout << "vcbld.json not found!" << std::endl;
    }
  }
  else
  {
    try
    {
      ConfClass confClass;
      command << "./" << confClass.outputDirectory() << "/release/"
              << confClass.binaryName();
      int systemRet = system(command.str().c_str());
      if (systemRet == -1)
      {
        std::cout << "An error occured while running the binary." << std::endl;
      }
    }
    catch (const std::exception &e)
    {
      std::cout << "vcbld.json not found!" << std::endl;
    }
  }
}

void available()
{
  try
  {
    ConfClass confClass;
    std::vector<fs::directory_entry> dirEntry;

    std::string vcpkgDirPath = confClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += confClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath)))
    {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));

      for (std::vector<fs::directory_entry>::const_iterator it =
               dirEntry.begin();
           it != dirEntry.end(); ++it)
      {
        if (((*it).path().filename().string()).at(0) != '.')
        {
          std::cout << ((*it).path().filename().string()) << std::endl;
        }
      }
    }
    else
    {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "package.json not found!" << std::endl;
  }
}

void search(const std::string &pkg)
{
  try
  {
    ConfClass confClass;
    std::vector<fs::directory_entry> dirEntry;

    std::string vcpkgDirPath = confClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += confClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath)))
    {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));

      for (std::vector<fs::directory_entry>::const_iterator it =
               dirEntry.begin();
           it != dirEntry.end(); ++it)
      {
        if (((*it).path().filename().string()).find(pkg) != std::string::npos)
        {
          std::cout << ((*it).path().filename().string()) << std::endl;
        }
      }
    }
    else
    {
      std::cout << "vcpkg packages not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  }
  catch (const std::exception &)
  {
  }
}

void includes() { gen::includePathGen(); }
void generate() { gen::cmakeGen(); }

void list()
{
  try
  {
    ConfClass confClass;
    for (std::vector<std::string>::iterator it = confClass.packageNames.begin();
         it != confClass.packageNames.end(); ++it)
    {
      std::cout << std::setw(4) << "Package name: " << *it
                << "\t\tPackage version: " << confClass.getVersion(*it)
                << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "vcpkg packages not found!" << std::endl;
    std::cout << "Please verify your vcpkg path." << std::endl;
  }
}

void add(const std::vector<std::string> &pkg)
{
  ConfClass confClass;
  for (std::vector<std::string>::const_iterator it = pkg.begin();
       it != pkg.end(); ++it)
  {
    std::string addDep = confClass.vcpkgDirPath() + "/" + "installed" + "/" +
                         confClass.architecture() + "/" + "share" + "/" + *it;

    if (fs::is_directory(static_cast<fs::path>(addDep)))
    {
      bool isExist = false;
      for (std::vector<std::string>::iterator jt =
               confClass.packageNames.begin();
           jt != confClass.packageNames.end(); ++jt)
      {
        if (*jt == *it)
        {
          std::cout << "Package already exists" << std::endl;
          isExist = true;
          break;
        }
        else
        {
          isExist = false;
        }
      }
      if (isExist != true)
      {
        confClass.include(*it);
        confClass.write();
      }
    }
    else
    {
      std::cout << "Package not found!" << std::endl;
      std::cout << "Please verify your vcpkg path." << std::endl;
    }
  }
}

void remove(const std::vector<std::string> &pkg)
{
  ConfClass confClass;
  for (std::vector<std::string>::const_iterator it = pkg.begin();
       it != pkg.end(); ++it)
  {
    for (std::vector<std::string>::iterator jt = confClass.packageNames.begin();
         jt != confClass.packageNames.end(); ++jt)
    {
      if (*jt == *it)
      {
        confClass.remove(*it);
        confClass.write();
        break;
      }
      else
      {
      }
    }
  }
}

void vcpkg(const std::string &vcpkgCmnds)
{
  ConfClass confClass;
  std::string temp =
      confClass.vcpkgDirPath() + "/" + "vcpkg" + " " + vcpkgCmnds;
  int systemRet = system(temp.c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while running vcpkg." << std::endl;
  }
}

void restore()
{
  ConfClass confClass;
  std::ostringstream pkg;

  for (std::vector<std::string>::iterator it = confClass.packageNames.begin();
       it != confClass.packageNames.end(); ++it)
  {
    pkg << *it << " ";
  }
  std::string instlCmnd = confClass.vcpkgDirPath() + "/" + "vcpkg" + " " +
                          "install" + " " + pkg.str();
  int systemRet = system(instlCmnd.c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while getting missing dependencies."
              << std::endl;
  }
}

void cmake(const std::string &cmakeArgs)
{
  ConfClass confClass;
  std::ostringstream cmakeCmnd;
  cmakeCmnd << "cd " << confClass.outputDirectory() << " && "
            << confClass.cmakePath()
            << " -DCMAKE_TOOLCHAIN_FILE=" << confClass.vcpkgDirPath()
            << "/scripts/buildsystems/vcpkg.cmake " << cmakeArgs << " .. ";
  int systemRet = system(cmakeCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while running cmake." << std::endl;
  }
}

void make(const std::string &makeArgs)
{
  ConfClass confClass;
  std::ostringstream makeCmnd;
  makeCmnd << "cd " << confClass.outputDirectory() << " && "
           << " " << confClass.makePath();
  int systemRet = system(makeCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while running make." << std::endl;
  }
}

std::string sinTriplet(const std::string &pkg)
{
  std::size_t found;
  std::string stripTrip;
  found = pkg.find("_");
  if (found != std::string::npos)
  {
    stripTrip = pkg.substr(0, found);
  }
  return stripTrip;
}

bool findPackage(const std::string &pkg)
{
  std::size_t found;
  std::string pkgName;
  found = pkg.find(pkg);
  if (found != std::string::npos)
  {
    std::cout << found;
    return true;
  }
  else
  {
    return false;
  }
}
} // namespace args
} // namespace vcbld
