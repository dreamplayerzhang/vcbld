#include "builder.h"

#include <algorithm>
#include <experimental/filesystem>
#include <iostream>
#include <iterator>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace vcbld
{

Builder::Builder(const std::string &buildType)
{
  this->confClass = new ConfClass();
  this->_buildType = buildType;
  if (!fs::exists("vcbld.json"))
  {
    std::cout << "Build configuration not found!" << std::endl;
    std::exit(1);
  }

  this->_dbgDir = this->confClass->outputDirectory().string() + "/" + "debug";
  this->_rlsDir = this->confClass->outputDirectory().string() + "/" + "release";
}

std::string Builder::compile()
{

  if (this->_buildType == "release")
  {
    this->_compileCommand << "cd " << this->_rlsDir << " && "
                          << this->confClass->compilerPath() << " -c "
                          << this->confClass->sourceFiles() << " "
                          << this->confClass->headerPaths() << " "
                          << this->confClass->compilerDefines() << " "
                          << this->confClass->compilerFlags() << " "
                          << "-std=" << this->confClass->language()
                          << this->confClass->standard() << " ";
  }
  else
  {
    this->_compileCommand << "cd " << this->_dbgDir << " && "
                          << this->confClass->compilerPath() << " -c -g "
                          << this->confClass->sourceFiles() << " "
                          << this->confClass->headerPaths() << " "
                          << this->confClass->compilerDefines() << " "
                          << this->confClass->compilerFlags() << " "
                          << "-std=" << this->confClass->language()
                          << this->confClass->standard() << " ";
  }
  int systemRet = system(this->_compileCommand.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while compiling." << std::endl;
  }
  return this->_compileCommand.str();
}

std::string Builder::appLink()
{
  std::vector<fs::directory_entry> dirEntry;
  std::string temp, tempPath;
  if (this->_buildType == "debug")
  {
    tempPath = this->_dbgDir;
  }
  else
  {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory(static_cast<fs::path>(tempPath)))
  {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()), dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {
      if (fs::path((*it).path().filename().string()).extension() == ".o" ||
          fs::path((*it).path().filename().string()).extension() == ".obj")
      {
        temp += " ";
        temp += (*it).path().filename().string();
      }
    }
  }

  if (this->_buildType == "release")
  {
    this->_appLinkCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass->compilerPath() << " -o "
                       << this->confClass->binaryName() << " " << temp << " "
                       << this->confClass->rlsLibPaths() << " "
                       << this->confClass->linkerFlags();
  }
  else
  {
    this->_appLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass->compilerPath() << " -o "
                       << this->confClass->binaryName() << " " << temp << " "
                       << this->confClass->dbgLibPaths() << " "
                       << this->confClass->linkerFlags();
  }
  int systemRet = system(this->_appLinkCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while linking." << std::endl;
  }
  return this->_appLinkCmnd.str();
}

std::string Builder::dylibLink()
{
  std::vector<fs::directory_entry> dirEntry;
  std::string temp, tempPath;
  if (this->_buildType == "debug")
  {
    tempPath = this->_dbgDir;
  }
  else
  {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory(static_cast<fs::path>(tempPath)))
  {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()), dirEntry.end());

    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {
      if (fs::path((*it).path().filename().string()).extension() == ".o")
      {
        temp += " ";
        temp += (*it).path().string();
      }
    }
  }

  std::string dylibArg, dylibExt;
  if (this->confClass->compilerPath().find("clang") != std::string::npos)
  {
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  }
  else
  {
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (this->_buildType == "release")
  {
    this->_libLinkCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass->compilerPath() << dylibArg << " -o "
                       << this->confClass->binaryName() << dylibExt << " "
                       << temp << " " << this->confClass->rlsLibPaths() << " "
                       << this->confClass->linkerFlags();
  }
  else
  {
    this->_libLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass->compilerPath() << dylibArg << " -o "
                       << this->confClass->binaryName() << dylibExt << " "
                       << temp << " " << this->confClass->dbgLibPaths() << " "
                       << this->confClass->linkerFlags();
  }
  int systemRet = system(this->_libLinkCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while linking." << std::endl;
  }
  return this->_libLinkCmnd.str();
}

std::string Builder::archive()
{
  std::vector<fs::directory_entry> dirEntry;
  std::string temp, tempPath;
  if (this->_buildType == "debug")
  {
    tempPath = this->_dbgDir;
  }
  else
  {
    tempPath = this->_rlsDir;
  }

  if (fs::is_directory(static_cast<fs::path>(tempPath)))
  {
    std::copy(fs::directory_iterator(tempPath), fs::directory_iterator(),
              back_inserter(dirEntry));
    std::sort(dirEntry.begin(), dirEntry.end());
    dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()), dirEntry.end());
    for (std::vector<fs::directory_entry>::iterator it = dirEntry.begin();
         it != dirEntry.end(); ++it)
    {
      if (fs::path((*it).path().filename().string()).extension() == ".o")
      {
        temp += " ";
        temp += (*it).path().string();
      }
    }
  }

  if (this->_buildType == "release")
  {
    this->_archiveCmnd << "cd " << this->_rlsDir << " && "
                       << this->confClass->archiverPath() << " rcs " << this->confClass->binaryName() << ".a"
                       << " " << temp;
  }
  else
  {
    this->_archiveCmnd << "cd " << this->_dbgDir << " && "
                       << this->confClass->archiverPath() << " rcs " << this->confClass->binaryName() << ".a"
                       << " " << temp;
  }
  int systemRet = system(this->_archiveCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while archiving." << std::endl;
  }
  return this->_archiveCmnd.str();
}

std::string Builder::getBldCommands()
{

  if (this->confClass->binaryType() == "app")
  {
    return this->compile() + "\n" + this->appLink();
  }
  else if (this->confClass->binaryType() == "staticLibrary")
  {
    return this->compile() + "\n" + this->archive();
  }
  else if (this->confClass->binaryType() == "sharedLibrary")
  {
    return this->compile() + "\n" + this->dylibLink();
  }
  else
  {
    std::cout << "Unknown binary type defined in vcbld.json" << std::endl;
    return "";
  }
}

void Builder::build()
{

  if (!fs::exists(this->confClass->outputDirectory()))
    fs::create_directory(this->confClass->outputDirectory());
  if (!fs::exists(this->confClass->outputDirectory().string() + "/" +
                  "release"))
    fs::create_directory(this->_rlsDir);
  if (!fs::exists(this->confClass->outputDirectory().string() + "/" + "debug"))
    fs::create_directory(this->_dbgDir);

  if (this->confClass->binaryType() == "app")
  {
    this->compile();
    this->appLink();
    try
    {
      this->copy();
    }
    catch (...)
    {
      std::cout << "Libraries exist in output directory" << std::endl;
    }
  }
  else if (this->confClass->binaryType() == "staticLibrary")
  {
    this->compile();
    this->archive();
  }
  else if (this->confClass->binaryType() == "sharedLibrary")
  {
    this->compile();
    this->dylibLink();
    try
    {
      this->copy();
    }
    catch (...)
    {
      std::cout << "Libraries exist in output directory" << std::endl;
    }
  }
  else
  {
    std::cout << "Unknown binary type defined in vcbld.json" << std::endl;
  }
}

void Builder::copy()
{
  std::string dbgLibPath = this->confClass->vcpkgDirPath() + "/" + "installed" + "/" + this->confClass->architecture() + "/" + "debug/lib";
  std::string rlsLibPath = this->confClass->vcpkgDirPath() + "/" + "installed" + "/" + this->confClass->architecture() + "/" + "lib";
  std::string fullName;
  for (std::vector<std::string>::iterator it = this->confClass->fullLibNames.begin(); it != this->confClass->fullLibNames.end(); ++it)
  {
    if (this->_buildType == "debug")
    {
      fullName = dbgLibPath + "/" + (*it);
    }
    else
    {
      fullName = rlsLibPath + "/" + (*it);
    }
    if (fs::exists(fullName))
    {
      if (this->_buildType == "debug")
      {
        fs::path temp = static_cast<fs::path>(fullName);
        if ((temp.filename().extension() != ".a" || temp.filename().extension() != ".lib") && (!fs::exists(this->_dbgDir + "/" + (*it))))
        {
          fs::copy(fullName, this->_dbgDir);
        }
      }
      else
      {
        fs::path temp = static_cast<fs::path>(fullName);
        if ((temp.filename().extension() != ".a" || temp.filename().extension() != ".lib") && (!fs::exists(this->_dbgDir + "/" + (*it))))
        {
          fs::copy(fullName, this->_rlsDir);
        }
      }
    }
  }
}

Builder::~Builder()
{
  delete confClass;
}
} // namespace vcbld
