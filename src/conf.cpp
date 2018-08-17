#include "conf.h"

#include <algorithm>
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
    this->_libsDirectory = vcbldJson["libsDirectory"];

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
  }
  catch (const json::parse_error e)
  {
    std::cerr << "Error reading vcbld.json." << std::endl;
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
  catch (const json::parse_error e)
  {
    std::cerr << "Error reading conf.json." << std::endl;
  }

  fs::path vcpkgPath;
  this->_vcpkgDirectory = confJson["vcpkgDirectory"];
  vcpkgPath = static_cast<std::string>(this->_vcpkgDirectory);
  this->_cCompilerPath = confJson["cCompilerPath"];
  this->_cppCompilerPath = confJson["cppCompilerPath"];
  this->_architecture = confJson["architecture"];
  this->_cmakePath = confJson["cmakePath"];
  this->_makePath = confJson["makePath"];
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
      std::cerr << "Failed to open package.json file : " << errno << std::endl;
    }
    for (json::iterator it = pkgsJson["packages"].begin();
         it != pkgsJson["packages"].end(); ++it)
    {
      this->packageNames.push_back(*it);
    }
  }
  catch (const json::parse_error e)
  {
    std::cerr << "Error reading package.json" << std::endl;
  }

  std::vector<fs::directory_entry> dirEntry;
  std::string arch = "_" + this->architecture() + ".list";
  std::string temp, temp2;
  size_t foundFile;
  std::string listsPath =
      this->vcpkgDirPath() + "/" + "installed" + "/" + +"vcpkg" + "/" + "info";
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

      for (std::vector<std::string>::iterator jt = this->packageNames.begin();
           jt != this->packageNames.end(); ++jt)
      {
        foundFile = (*it).path().filename().string().find(*jt);
        if (foundFile != std::string::npos && foundFile == 0)
        {
          std::string fileName = (*it).path().filename().string();
          std::string line;
          std::ifstream ifs((*it).path());
          try
          {
            if (ifs.is_open())
            {
              while (!ifs.eof())
              {
                getline(ifs, line);
                if (findLib(line) != "")
                {
                  libs.push_back(stripLibName(findLib(line)));
                }
              }
            }
          }
          catch (...)
          {
            // fail quietly!
          }
        }
        std::sort(libs.begin(), libs.end());
        libs.erase(std::unique(libs.begin(), libs.end()), libs.end());
      }
    }
  }

  for (std::vector<std::string>::iterator it = this->libs.begin();
       it != this->libs.end(); ++it)
  {
    std::string libName = *it;
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
  if (this->boostComponents.size() > 1)
  {
    this->_cmakeOutput << "#Find "
                       << "Boost components"
                       << "\n"
                       << "find_package(Boost COMPONENTS ";
    for (std::vector<std::string>::iterator it = this->boostComponents.begin();
         it != this->boostComponents.end(); ++it)
    {
      this->_cmakeOutput << *it << " ";
    }
    this->_cmakeOutput << "REQUIRED)\n"
                       << "set(dbgLIBS ${dbgLIBS} ${Boost_LIBRARIES})\n"
                       << "set(rlsLIBS ${rlsLIBS} ${Boost_LIBRARIES})\n\n";
  }
  if (this->QtComponents.size() > 1)
  {
    this->_cmakeOutput << "#Find "
                       << "Qt components"
                       << "\n"
                       << "find_package(Qt5 COMPONENTS ";
    for (std::vector<std::string>::iterator jt = this->QtComponents.begin();
         jt != this->QtComponents.end(); ++jt)
    {
      this->_cmakeOutput << *jt << " ";
    }
    this->_cmakeOutput << "REQUIRED)\n"
                       << "set(dbgLIBS ${dbgLIBS} ${Qt5_LIBRARIES})\n"
                       << "set(rlsLIBS ${rlsLIBS} ${Qt5_LIBRARIES})\n\n";
  }
}

void ConfClass::write()
{
  json pkgsJson;

  for (std::vector<std::string>::iterator it = this->packageNames.begin();
       it != this->packageNames.end(); ++it)
  {
    pkgsJson["packages"].push_back(*it);
  }
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
    std::cerr << "Failed to open file : " << errno << std::endl;
  }
  std::string line;
  bool isEmpty = false;
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
    std::cerr << "Failed to open file : " << errno << std::endl;
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
  temp << " -I" << this->sourceDirectory() << " -I" << this->includeDirectory()
       << " -I" << this->vcpkgDirPath() << "/"
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

  return stripExt;
}

std::string ConfClass::dbgLibPaths()
{
  std::ostringstream temp;
  std::string localDbgLibs = this->libsDirectory().string() + "/" + "debug";
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
  std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                           this->architecture() + "/" + "debug" + "/" + "lib";
  for (std::vector<std::string>::iterator it = this->libs.begin();
       it != this->libs.end(); ++it)
  {
    temp << " -L" << dbgLibPath << " "
         << "-l" << *it;
  }
  return temp.str();
}

std::string ConfClass::rlsLibPaths()
{
  std::ostringstream temp;
  std::string localRlsLibs = this->libsDirectory().string() + "/" + "lib";
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

  for (std::vector<std::string>::iterator it = this->packageNames.begin();
       it != this->packageNames.end(); ++it)
  {
    std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" +
                             this->architecture() + "/" + "lib";
    for (std::vector<std::string>::iterator it = this->libs.begin();
         it != this->libs.end(); ++it)
    {
      temp << " -L" << dbgLibPath << " "
           << "-l" << *it;
    }
  }
  return temp.str();
}

void ConfClass::include(const std::string &pkgName)
{
  this->packageNames.push_back(pkgName);
}

void ConfClass::remove(const std::string &pkgName)
{
  for (std::vector<std::string>::iterator it = this->packageNames.begin();
       it != this->packageNames.end();)
  {
    if (*it == pkgName)
    {
      it = this->packageNames.erase(it);
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
    boostComponents.push_back(stripComponent);
    return true;
  }
  else if (foundQt != std::string::npos && foundBoost == 0)
  {
    stripComponent = libName.substr(4, libName.length());
    QtComponents.push_back(stripComponent);
    return true;
  }
  else
  {
    return false;
  }
}
std::string ConfClass::cmakeOutput() const { return this->_cmakeOutput.str(); }
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
std::string ConfClass::cmakePath() const { return _cmakePath; }
std::string ConfClass::makePath() const { return _makePath; }
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
} // namespace vcbld
