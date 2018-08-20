#include "builder.h"

#include <algorithm>
#include <errno.h>
#include <experimental/filesystem>
#include <iostream>
#include <iterator>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace vcbld
{

Builder::Builder(const std::string &buildType) : _buildType(buildType), PrepClass()
{
  if (!fs::exists("vcbld.json"))
  {
    std::cout << "Build configuration not found!" << std::endl;
    std::exit(1);
  }

  this->_dbgDir = this->outputDirectory().string() + "/" + "debug";
  this->_rlsDir = this->outputDirectory().string() + "/" + "release";
}

void Builder::compile()
{

  if (this->_buildType == "release")
  {
    this->_compileCommand << "cd " << this->_rlsDir << " && "
                          << this->compilerPath() << " "
                          << this->headerPaths() << " "
                          << this->compilerFlags() << " "
                          << this->compilerDefines() << " "
                          << "-std=" << this->language()
                          << this->standard() << " -c "
                          << this->sourceFiles();
  }
  else
  {
    this->_compileCommand << "cd " << this->_dbgDir << " && "
                          << this->compilerPath() << " "
                          << this->headerPaths() << " "
                          << this->compilerFlags() << " "
                          << this->compilerDefines() << " -g "
                          << "-std=" << this->language()
                          << this->standard() << " -c "
                          << this->sourceFiles();
  }
  int systemRet = system(this->_compileCommand.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while compiling." << std::endl;
  }
}

void Builder::appLink()
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
                       << this->compilerPath() << " -o "
                       << this->binaryName() << " " << temp << " "
                       << this->rlsLibPaths() << " "
                       << this->linkerFlags();
  }
  else
  {
    this->_appLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->compilerPath() << " -o "
                       << this->binaryName() << " " << temp << " "
                       << this->dbgLibPaths() << " "
                       << this->linkerFlags();
  }
  int systemRet = system(this->_appLinkCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while linking." << std::endl;
  }
}

void Builder::dylibLink()
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
  if (this->compilerPath().find("clang") != std::string::npos)
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
                       << this->compilerPath() << dylibArg << " -o "
                       << this->binaryName() << dylibExt << " "
                       << temp << " " << this->rlsLibPaths() << " "
                       << this->linkerFlags();
  }
  else
  {
    this->_libLinkCmnd << "cd " << this->_dbgDir << " && "
                       << this->compilerPath() << dylibArg << " -o "
                       << this->binaryName() << dylibExt << " "
                       << temp << " " << this->dbgLibPaths() << " "
                       << this->linkerFlags();
  }
  int systemRet = system(this->_libLinkCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while linking." << std::endl;
  }
}

void Builder::archive()
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
                       << this->archiverPath() << " rcs " << this->binaryName() << ".a"
                       << " " << temp;
  }
  else
  {
    this->_archiveCmnd << "cd " << this->_dbgDir << " && "
                       << this->archiverPath() << " rcs " << this->binaryName() << ".a"
                       << " " << temp;
  }
  int systemRet = system(this->_archiveCmnd.str().c_str());
  if (systemRet == -1)
  {
    std::cout << "An error occured while archiving." << std::endl;
  }
}

std::string Builder::getBldCommands()
{
  if (this->binaryType() == "app")
  {
    return this->_compileCommand.str() + "\n" + this->_appLinkCmnd.str();
  }
  else if (this->binaryType() == "staticLibrary")
  {
    return this->_compileCommand.str() + "\n" + this->_archiveCmnd.str();
  }
  else if (this->binaryType() == "sharedLibrary")
  {
    return this->_compileCommand.str() + "\n" + this->_libLinkCmnd.str();
  }
  else
  {
    std::cout << "Unknown binary type defined in vcbld.json." << std::endl;
    return "";
  }
}

void Builder::build()
{

  if (!fs::exists(this->outputDirectory()))
    fs::create_directory(this->outputDirectory());
  if (!fs::exists(this->outputDirectory().string() + "/" +
                  "release"))
    fs::create_directory(this->_rlsDir);
  if (!fs::exists(this->outputDirectory().string() + "/" + "debug"))
    fs::create_directory(this->_dbgDir);

  if (this->binaryType() == "app")
  {
    try
    {
      std::cout << "Compiling...\n";
      this->compile();
      std::cout << "Done!" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try
    {
      std::cout << "Linking...\n";
      this->appLink();
      std::cout << "Done!" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Linking failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try
    {
      this->copy();
    }
    catch (const std::exception &e)
    {
      std::cout << "Libraries exist in output directory." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }
  else if (this->binaryType() == "staticLibrary")
  {
    try
    {
      std::cout << "Compiling...\n";
      this->compile();
      std::cout << "Done!" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try
    {
      std::cout << "Archiving...\n";
      this->archive();
      std::cout << "Done!" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Archiving failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
  }
  else if (this->binaryType() == "sharedLibrary")
  {
    try
    {
      std::cout << "Compiling...\n";
      this->compile();
      std::cout << "Done!" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Compilation failed!" << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
    try
    {
      std::cout << "Linking...\n";
      this->dylibLink();
      std::cout << "Done!" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << "Linking failed!" << std::endl;
      std::cerr << e.what() << errno << std::endl;
    }
    try
    {
      this->copy();
    }
    catch (const std::exception &e)
    {
      std::cout << "Libraries exist in output directory." << std::endl;
      std::cerr << e.what() << " " << errno << std::endl;
    }
  }
  else
  {
    std::cout << "Unknown binary type defined in vcbld.json." << std::endl;
  }
}

void Builder::copy()
{
  std::string dbgLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" + this->architecture() + "/" + "debug/lib";
  std::string rlsLibPath = this->vcpkgDirPath() + "/" + "installed" + "/" + this->architecture() + "/" + "lib";
  std::string fullName;
  for (std::vector<std::string>::iterator it = this->fullLibNames().begin(); it != this->fullLibNames().end(); ++it)
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
        if ((*it).find(".a") == std::string::npos || (*it).find(".a") == std::string::npos) 
        {
          if (!fs::exists(this->_dbgDir + "/" + (*it)))
          {
            fs::copy(fullName, this->_dbgDir);
          }
        }
      }
      else
      {
        if ((*it).find(".a") == std::string::npos || (*it).find(".a") == std::string::npos) 
        {
          if (!fs::exists(this->_rlsDir + "/" + (*it)))
          {
            fs::copy(fullName, this->_rlsDir);
          }
        }
      }
    }
  }
}

} // namespace vcbld
