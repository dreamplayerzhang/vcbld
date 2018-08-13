#ifndef CMAKE_H
#define CMAKE_H 

#include <boost/filesystem.hpp>
#include <sstream>
#include <string>

#include "conf.h"

namespace fs = boost::filesystem;

namespace vcbld{

    class CmakeClass
    {
    public:
        explicit CmakeClass(const fs::path &vcbldPath);
        bool hasComponents(const std::string &libName);
        std::string output() const;

        ~CmakeClass();
    private:
        ConfClass *confClass;
        std::ostringstream _output;
        std::vector<std::string> boostComponents;
        std::vector<std::string> QtComponents;
        
    };
    

}


#endif // !CMAKE_H