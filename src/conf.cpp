#include "conf.h"

#include <algorithm>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>

#include "init.h"

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

namespace vcbld
{

ConfClass::ConfClass()
{
  json vcbldJson, confJson, pkgsJson;
  this->_projPath = fs::current_path();

  if (!fs::exists("vcbld.json"))
  {
    std::cout << "vcbld.json not found in the current directory" << std::endl;
    std::exit(1);
  }

  try
  {
    std::ifstream vcbldInput("vcbld.json");
    if (vcbldInput.is_open())
    {
      vcbldJson = json::parse(vcbldInput);
      vcbldInput.close();
    }
    else
    {
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
    this->_libDirectory = vcbldJson["libDirectory"];

    for (json::iterator it = vcbldJson["compilerDefines"].begin();
         it != vcbldJson["compilerDefines"].end(); ++it)
    {
      this->_compilerDefines << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["compilerFlags"].begin();
         it != vcbldJson["compilerFlags"].end(); ++it)
    {
      this->_compilerFlags << " " << *it << " ";
    }

    for (json::iterator it = vcbldJson["linkerFlags"].begin();
         it != vcbldJson["linkerFlags"].end(); ++it)
    {
      this->_linkerFlags << " " << *it << " ";
    }
  }
  catch (const json::parse_error &e)
  {
    std::cerr << "Error reading vcbld.json. " << e.what() << std::endl;
  }

  try
  {
    std::ifstream confInput("conf.json");
    if (confInput.is_open())
    {
      confJson = json::parse(confInput);
      confInput.close();
    }
    else
    {
      std::cerr << "Failed to open conf.json file : " << errno << std::endl;
    }
  }
  catch (const json::parse_error &e)
  {
    std::cerr << "Error reading conf.json." << e.what() << std::endl;
  }

  fs::path vcpkgPath;
  this->_vcpkgDirectory = confJson["vcpkgDirectory"];
  vcpkgPath = static_cast<std::string>(this->_vcpkgDirectory);
  this->_cCompilerPath = confJson["cCompilerPath"];
  this->_cppCompilerPath = confJson["cppCompilerPath"];
  this->_architecture = confJson["architecture"];
  this->_cmakePath = confJson["cmakePath"];
  this->_makePath = confJson["makePath"];
  this->_archiverPath = confJson["archiverPath"];
  this->_vcpkgDirPath = vcpkgPath.string();

  try
  {
    std::ifstream pkgsInput("package.json");
    if (pkgsInput.is_open())
    {
      pkgsJson = json::parse(pkgsInput);
      pkgsInput.close();
    }
    else
    {
      std::cerr << "Failed to open package.json file: " << errno << std::endl;
    }
    for (json::iterator it = pkgsJson["packages"].begin();
         it != pkgsJson["packages"].end(); ++it)
    {
      this->_packageNames.push_back(*it);
    }
  }
  catch (const json::parse_error &e)
  {
    std::cerr << "Error reading package.json: " << e.what() << std::endl;
  }

  std::vector<fs::directory_entry> dirEntry;
  std::string arch = "_" + this->architecture() + ".list";
  std::string temp, temp2;
  size_t foundFile;
  std::string listsPath = this->vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
  if (fs::is_directory(static_cast<fs::path>(listsPath)))
  {
    std::copy(fs::directory_iterator(listsPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {

      for (std::vector<std::string>::iterator jt = this->_packageNames.begin();
           jt != this->_packageNames.end(); ++jt)
      {
        foundFile = (*it).path().filename().string().find(*jt);
        if (foundFile != std::string::npos && foundFile == 0)
        {
          std::string fileName = (*it).path().filename().string();
          std::string line;
          try
          {
            std::ifstream ifs((*it).path());
            if (ifs.is_open())
            {
              while (!ifs.eof())
              {
                getline(ifs, line);
                if (findLib(line) != "")
                {
                  std::string libFound = findLib(line);
                  this->_fullLibNames.push_back(libFound);
                }
              }
            }
          }
          catch (...)
          {
            // fail quietly!
          }
        }
        std::sort(this->_fullLibNames.begin(), this->_fullLibNames.end());
        this->_fullLibNames.erase(std::unique(this->_fullLibNames.begin(), this->_fullLibNames.end()), this->_fullLibNames.end());
      }
    }
  }
}

void ConfClass::write()
{
  json pkgsJson;

  for (std::vector<std::string>::iterator it = this->_packageNames.begin();
       it != this->_packageNames.end(); ++it)
  {
    pkgsJson["packages"].push_back(*it);
  }
  try
  {
    std::ofstream pkgsOutput("package.json");
    if (pkgsOutput.is_open())
    {
      pkgsOutput << std::setw(4) << pkgsJson;
      pkgsOutput.flush();
      pkgsOutput.close();
      std::cout << "Packages updated successfully." << std::endl;
    }
    else
    {
      std::cout << "Packages updated successfully." << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Failed to open file: " << e.what() << " " << errno
              << std::endl;
  }
  std::string line;
  bool isEmpty = false;
  try
  {
    std::ifstream pkgsInput("package.json");
    if (pkgsInput.is_open())
    {
      std::getline(pkgsInput, line);
      if (line == "null")
      {
        isEmpty = true;
        pkgsInput.close();
      }
    }
    else
    {
      std::cout << "Packages updated successfully." << std::endl;
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Failed to open file : " << e.what() << " " << errno
              << std::endl;
  }
  if (isEmpty == true)
  {
    fs::remove("package.json");
    init::init(this->binaryType());
  }
}

std::string ConfClass::sourceFiles() const
{
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = this->_sourceDirectory;
  if (fs::is_directory(static_cast<fs::path>(tempPath)))
  {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {
      if (fs::path((*it).path().filename().string()).extension() == ".cpp" ||
          fs::path((*it).path().filename().string()).extension() == ".rc" ||
          fs::path((*it).path().filename().string()).extension() == ".c" ||
          fs::path((*it).path().filename().string()).extension() == ".cxx" ||
          fs::path((*it).path().filename().string()).extension() == ".qrc" ||
          fs::path((*it).path().filename().string()).extension() == ".ui")
      {
        temp << this->_projPath.c_str() << "/" << tempPath + "/"
             << (*it).path().filename().string() << " ";
      }
    }
  }
  return temp.str();
}

std::string ConfClass::sourceFilesSinPath() const
{
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath;
  std::ostringstream temp;
  tempPath = this->_sourceDirectory;
  if (fs::is_directory(static_cast<fs::path>(tempPath)))
  {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {
      if (fs::path((*it).path().filename().string()).extension() == ".cpp" ||
          fs::path((*it).path().filename().string()).extension() == ".rc" ||
          fs::path((*it).path().filename().string()).extension() == ".c" ||
          fs::path((*it).path().filename().string()).extension() == ".cxx")
      {
        temp << "\"" << (*it).path().filename().string() << "\" ";
      }
    }
  }
  return temp.str();
}

std::string ConfClass::compilerPath() const
{
  if (this->_language.at(2) == '+')
  {
    return this->_cppCompilerPath;
  }
  else
  {
    return this->_cCompilerPath;
  }
}

std::string ConfClass::getVersion(const std::string &pkgName)
{
  std::vector<fs::directory_entry> dirEntry;
  std::string tempPath =
      this->vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
  std::string temp, temp2;
  size_t foundArch, foundPkg;
  std::string arch = "_" + this->architecture() + ".list";
  std::string pkg = pkgName + "_";
  if (fs::is_directory(static_cast<fs::path>(tempPath)))
  {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                   dirEntry.end());
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {
      std::string fileName = (*it).path().filename().string();
      foundPkg = fileName.find(pkg);
      if (foundPkg != std::string::npos && foundPkg == 0)
      {
        temp2 = (*it).path().filename().string().substr(pkg.length(),
                                                        fileName.length());
      }
    }
  }
  foundArch = temp2.find(arch);
  temp = temp2.substr(0, foundArch);
  return temp;
}

std::string ConfClass::headerPaths()
{
  std::ostringstream temp;
  temp << " -I" << fs::canonical(this->sourceDirectory()) << " -I"
       << fs::canonical(this->outputDirectory());
  if (this->includeDirectory() != "")
  {
    temp << " -I" << fs::canonical(this->includeDirectory());
  }
  temp << " -I" << this->vcpkgDirPath() << "/"
       << "installed"
       << "/" << this->architecture() << "/"
       << "include";
  return temp.str();
}

std::string ConfClass::stripLibName(const std::string &lib)
{
  std::size_t found;
  std::string stripLib;
  std::string stripExt;
  found = lib.find("lib");
  if (found != std::string::npos && found == 0)
  {
    stripLib = lib.substr(3, lib.length());
  }
  found = stripLib.find(".a");
  if (found != std::string::npos)
  {
    stripExt = stripLib.substr(0, (stripLib.length() - 2));
  }
  found = stripLib.find(".so");
  if (found != std::string::npos)
  {
    stripExt = stripLib.substr(0, (stripLib.length() - 3));
  }
  found = stripLib.find(".dylib");
  if (found != std::string::npos)
  {
    stripExt = stripLib.substr(0, (stripLib.length() - 6));
  }
  found = stripLib.find(".dll");
  if (found != std::string::npos)
  {
    stripExt = stripLib.substr(0, (stripLib.length() - 4));
  }
  found = stripLib.find(".lib");
  if (found != std::string::npos)
  {
    stripExt = stripLib.substr(0, (stripLib.length() - 4));
  }

  return stripExt;
}

std::string ConfClass::dbgLibPaths()
{
  std::ostringstream temp;
  if (this->libDirectory().string() != "")
  {
    std::string localDbgLibs = this->libDirectory().string() + "/" + "debug";
    std::vector<fs::directory_entry> dirEntry;

    if (fs::is_directory(static_cast<fs::path>(localDbgLibs)))
    {
      std::copy(fs::directory_iterator(localDbgLibs), fs::directory_iterator(),
                std::back_inserter(dirEntry));
      std::sort(dirEntry.begin(), dirEntry.end());
      dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                     dirEntry.end());

      for (std::vector<fs::directory_entry>::iterator jt = dirEntry.begin();
           jt != dirEntry.end(); ++jt)
      {
        if (stripLibName((*jt).path().filename().string()).at(0) != '.')
        {
          temp << " -L" << (*jt).path().parent_path().string() << " "
               << " -l" << stripLibName((*jt).path().filename().string());
        }
      }
    }
  }
  std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                           this->architecture() + "/" + "debug" + "/" + "lib";
  for (std::vector<std::string>::iterator it = this->_fullLibNames.begin();
       it != this->_fullLibNames.end(); ++it)
  {
    temp << " -L" << dbgLibPath << " "
         << "-l" << stripLibName(*it);
  }
  return temp.str();
}

std::string ConfClass::rlsLibPaths()
{
  std::ostringstream temp;
  if (this->libDirectory().string() != "")
  {
    std::string localRlsLibs = this->libDirectory().string() + "/" + "release";
    std::vector<fs::directory_entry> dirEntry;

    if (fs::is_directory(static_cast<fs::path>(localRlsLibs)))
    {
      std::copy(fs::directory_iterator(localRlsLibs), fs::directory_iterator(),
                std::back_inserter(dirEntry));
      std::sort(dirEntry.begin(), dirEntry.end());
      dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                     dirEntry.end());

      for (std::vector<fs::directory_entry>::iterator jt = dirEntry.begin();
           jt != dirEntry.end(); ++jt)
      {
        if (stripLibName((*jt).path().filename().string()).at(0) != '.')
        {
          temp << " -L" << (*jt).path().parent_path().string() << " "
               << " -l" << stripLibName((*jt).path().filename().string());
        }
      }
    }
  }
  for (std::vector<std::string>::iterator it = this->_packageNames.begin();
       it != this->_packageNames.end(); ++it)
  {
    std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                             this->architecture() + "/" + "lib";
    for (std::vector<std::string>::iterator it = this->_fullLibNames.begin();
         it != this->_fullLibNames.end(); ++it)
    {
      temp << " -L" << dbgLibPath << " "
           << "-l" << stripLibName(*it);
    }
  }
  return temp.str();
}

void ConfClass::include(const std::string &pkgName)
{
  this->_packageNames.push_back(pkgName);
}

void ConfClass::remove(const std::string &pkgName)
{
  for (std::vector<std::string>::iterator it = this->_packageNames.begin();
       it != this->_packageNames.end();)
  {
    if (*it == pkgName)
    {
      it = this->_packageNames.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

bool ConfClass::hasComponents(const std::string &libName)
{
  std::size_t foundBoost, foundQt;
  std::string stripPackage;
  std::string stripComponent;
  foundBoost = libName.find("boost");
  foundQt = libName.find("Qt");
  if (foundBoost != std::string::npos && foundBoost == 0)
  {
    stripComponent = libName.substr(6, libName.length());
    _boostComponents.push_back(stripComponent);
    return true;
  }
  else if (foundQt != std::string::npos && foundBoost == 0)
  {
    stripComponent = libName.substr(4, libName.length());
    this->_QtComponents.push_back(stripComponent);
    return true;
  }
  else
  {
    return false;
  }
}
std::string ConfClass::cmakeOutput()
{
  for (std::vector<std::string>::iterator it = this->_fullLibNames.begin();
       it != this->_fullLibNames.end(); ++it)
  {
    std::string libName = stripLibName(*it);
    std::string module = libName;
    module[0] = toupper(module[0]);
    if (!hasComponents(libName))
    {
      this->_cmakeOutput << "#Find " << libName << "\n"
                         << "find_library(" << libName << "_DBG NAMES "
                         << libName << " HINTS "
                         << "${DBG_LIB_PATH})\n"
                         << "find_library(" << libName << "_RLS NAMES "
                         << libName << " HINTS "
                         << "${RLS_LIB_PATH})\n"
                         << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n"
                         << "set(rlsLIBS ${rlsLIBS} ${" << libName
                         << "_RLS})\n\n";
    }
  }
  if (this->_boostComponents.size() > 1)
  {
    this->_cmakeOutput << "#Find "
                       << "Boost components"
                       << "\n"
                       << "find_package(Boost COMPONENTS ";
    for (std::vector<std::string>::iterator it = this->_boostComponents.begin();
         it != this->_boostComponents.end(); ++it)
    {
      this->_cmakeOutput << *it << " ";
    }
    this->_cmakeOutput << "REQUIRED)\n"
                       << "set(dbgLIBS ${dbgLIBS} ${Boost_LIBRARIES})\n"
                       << "set(rlsLIBS ${rlsLIBS} ${Boost_LIBRARIES})\n\n";
  }
  if (this->_QtComponents.size() > 1)
  {
    this->_cmakeOutput << "#Find "
                       << "Qt components"
                       << "\n"
                       << "find_package(Qt5 COMPONENTS ";
    for (std::vector<std::string>::iterator jt = this->_QtComponents.begin();
         jt != this->_QtComponents.end(); ++jt)
    {
      this->_cmakeOutput << *jt << " ";
    }
    this->_cmakeOutput << "REQUIRED)\n"
                       << "set(dbgLIBS ${dbgLIBS} ${Qt5_LIBRARIES})\n"
                       << "set(rlsLIBS ${rlsLIBS} ${Qt5_LIBRARIES})\n\n";
  }
  return this->_cmakeOutput.str();
}

std::string ConfClass::findLib(const std::string &line)
{
  std::string libName;
  size_t found;
  found = line.find("debug/lib/");
  if (found != std::string::npos)
  {
    libName = line.substr(found + 10, line.length());
  }
  return libName;
}
std::string ConfClass::compilerDefines() const
{
  return this->_compilerDefines.str();
}
std::string ConfClass::compilerFlags() const
{
  return this->_compilerFlags.str();
}
std::string ConfClass::linkerFlags() const
{
  return this->_compilerFlags.str();
}

std::vector<std::string> &ConfClass::packageNames()
{
  return this->_packageNames;
}

std::vector<std::string> &ConfClass::fullLibNames()
{
  return this->_fullLibNames;
}

std::string ConfClass::cmakePath() const { return _cmakePath; }
std::string ConfClass::makePath() const { return _makePath; }
std::string ConfClass::archiverPath() const { return _archiverPath; }
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
fs::path ConfClass::libDirectory() const { return _libDirectory; }
std::string ConfClass::architecture() const { return _architecture; }
} // namespace vcbld
