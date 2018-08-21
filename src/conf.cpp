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
  json vcbldJson, confJson;
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

    try
    {
      this->_version = vcbldJson["version"];
    }
    catch (...)
    {
      this->_version = "0.1.0";
    }

    try
    {
      this->_language = vcbldJson["language"];
    }
    catch (...)
    {
      this->_language = "c++";
    }

    try
    {
      this->_standard = vcbldJson["standard"];
    }
    catch (...)
    {
      this->_standard = "11";
    }

    this->_binaryName = vcbldJson["binaryName"];
    this->_binaryType = vcbldJson["binaryType"];

    try
    {
      this->_outputDirectory = vcbldJson["outputDirectory"];
    }
    catch (...)
    {
      this->_outputDirectory = "bin";
    }

    this->_sourceDirectory = vcbldJson["sourceDirectory"];

    try
    {
      this->_includeDirectory = vcbldJson["includeDirectory"];
    }
    catch (...)
    {
      this->_includeDirectory = "";
    }

    try
    {
      this->_libDirectory = vcbldJson["libDirectory"];
    }
    catch (...)
    {
      this->_libDirectory = "";
    }

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
  catch (...)
  {
    std::cerr << "Error reading vcbld.json. " << std::endl;
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
  catch (...)
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
  this->_archiverPath = confJson["archiverPath"];
  this->_vcpkgDirPath = vcpkgPath.string();
}

fs::path ConfClass::projPath() const { return this->_projPath; }

std::string ConfClass::compilerPath() const
{
  if (this->_language.find("++") != std::string::npos)
  {
    return this->_cppCompilerPath;
  }
  else
  {
    return this->_cCompilerPath;
  }
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
  return this->_linkerFlags.str();
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
