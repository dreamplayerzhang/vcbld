#include "cmake.h"

#include <iterator>
#include <vector>

#include "sstream"

namespace vcbld {
CmakeClass::CmakeClass(const fs::path &vcbldPath) {
  this->confClass = new ConfClass(vcbldPath);
  for (std::vector<std::string>::iterator it = this->confClass->libs.begin();
       it != this->confClass->libs.end(); ++it) {
    std::string libName = *it;
    std::string module = libName;
    module[0] = toupper(module[0]);
    if (!hasComponents(libName)) {
      this->_output << "#Find " << libName << "\n"
                    << "find_library(" << libName << "_DBG NAMES " << libName
                    << " HINTS "
                    << "${DBG_LIB_PATH})\n"
                    << "find_library(" << libName << "_RLS NAMES " << libName
                    << " HINTS "
                    << "${RLS_LIB_PATH})\n"
                    << "set(dbgLIBS ${dbgLIBS} ${" << libName << "_DBG})\n"
                    << "set(rlsLIBS ${rlsLIBS} ${" << libName << "_RLS})\n\n";
    }
  }
  if (this->boostComponents.size() > 1) {
    this->_output << "#Find "
                  << "Boost components"
                  << "\n"
                  << "find_package(Boost COMPONENTS ";
    for (std::vector<std::string>::iterator it = this->boostComponents.begin();
         it != this->boostComponents.end(); ++it) {
      this->_output << *it << " ";
    }
    this->_output << "REQUIRED)\n"
                  << "set(dbgLIBS ${dbgLIBS} ${Boost_LIBRARIES})\n"
                  << "set(rlsLIBS ${rlsLIBS} ${Boost_LIBRARIES})\n\n";
  }
  if (this->QtComponents.size() > 1) {
    this->_output << "#Find "
                  << "Qt components"
                  << "\n"
                  << "find_package(Qt5 COMPONENTS ";
    for (std::vector<std::string>::iterator jt = this->QtComponents.begin();
         jt != this->QtComponents.end(); ++jt) {
    this->_output << *jt << " ";
    }
    this->_output << "REQUIRED)\n"
                  << "set(dbgLIBS ${dbgLIBS} ${Qt5_LIBRARIES})\n"
                  << "set(rlsLIBS ${rlsLIBS} ${Qt5_LIBRARIES})\n\n";
  }
}

bool CmakeClass::hasComponents(const std::string &libName) {
  std::size_t foundBoost, foundQt;
  std::string stripPackage;
  std::string stripComponent;
  foundBoost = libName.find("boost");
  foundQt = libName.find("Qt");
  if (foundBoost != std::string::npos && foundBoost == 0) {
    stripComponent = libName.substr(6, libName.length());
    boostComponents.push_back(stripComponent);
    return true;
  } else if (foundQt != std::string::npos && foundBoost == 0) {
    stripComponent = libName.substr(4, libName.length());
    QtComponents.push_back(stripComponent);
    return true;
  } else {
    return false;
  }
}
std::string CmakeClass::output() const { return this->_output.str(); }
CmakeClass::~CmakeClass() { delete confClass; }
} // namespace vcbld