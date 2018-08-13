# vcbld
A build system to work with vcpkg.
It works for the moment on posix compliant systems, i.e. linux and mac os x. It runs on windows if you're using MinGW or MSYS. It has a minimal build system which works for small single hierarchy projects which allows for faster prototyping, however it has support for cmake and can be used to generate CMakeLists.txt files which can be seperately run using cmake and used to generate other build systems.
`
SDK commands:
    setup                           Creates a conf.json in the vcbld directory.
    new     [app|dylib|statlib]     Starts a new project.
    restore                         Restore missing dependencies in a project.
    build   [debug|release]         Builds the project.
    clean                           Clean build output(s).
    run     [debug|release]         Executes the project.
    includes                        Generates includePath.json file.
    gen                             Generates CMakeLists.txt file.
    list                            Shows a list of packages added to the project.
    available                       Shows a list of installed vcpkg packages.
    search                          Search within installed vcpkg packages.
    add                             Add package to the project.
    remove                          Remove package from the project.
    vcpkg   [vcpkg command]         Runs vcpkg commands.
    cmake   [cmake arguments]       Runs cmake in the output directory generating makefiles.
    make                            Runs make in the output directory.
    help                            Show help.

    --version                       Current vcbld version in use.
`
With a first download of a prebuilt vcbld executable, or after compiling one from source, the first thing to do is run vcbld setup on the command line. This will generate a conf.json file in the directory of the executable. It contains several default variables depending on the operating system, which can also be changed if need be. A conf.json file will look like the following:
```json
{
	"cCompilerPath" : "/usr/bin/clang",
	"cppCompilerPath" : "/usr/bin/clang++",
	"vcpkgDirectory" : "/Users/mohammedalyousef/vcpkg",
	"architecture" : "x64-osx",
	"cmakePath" : "/usr/local/bin/cmake",
    "makePath" : "/usr/bin/make"
}
```
The conf,json is not supposed to be pushed to version control repository since it's dependant on local variables.
The first 2 variables are the paths of the c and c++ compilers. The default compilers for mac os x is clang, while for linux it would be gcc and g++ respectively. On windows, the default compiler is the MinGW gcc.
The vcpkgDirectory variable is the parent directory of the vcpkg executable you wish to choose for your project. It defaults to the home path on linux and mac os x, while on windows it defaults to C:/Program Files/vcpkg.

The architecture depends on the host operating system. You can check the vcpkg triplet documentation here:
https://github.com/Microsoft/vcpkg/blob/master/docs/users/triplets.md
Although vcbld doesn't support cross-compilation on the same machine, vcbld projects can be cross-compiled on different operating systems.

cmakePath and makePath are your installation locations of cmake and make. vcbld supports building using cmake and make within the executable. It can also generate CMakeLists.txt files which can be seperately run using cmake and used to generate other build systems.

A new project will have a vcbld.json and package.json files in project directory.
A vcbld.json would look like the following (without the comments):
```json
{
	"projectName" : "myProj", // Name of the project.
	"version" : "0.1.0",
	"language" : "c++", 
	"standard" : "11", // The minimum supported standards of both of c++ and c.
	"binaryType" : "app", // or "staticLibrary" or "sharedLibrary"
	"binaryName" : "myApp", // Name of the resulting binary, use a "lib" prefix if it were a library.
    "sourceDirectory" : "./src", // Relative or absolute path of your source directory
    "includeDirectory" : "./include",
	"libsDirectory" : "./libs",
    "outputDirectory" : "./bin", // Relative or absolute path to your output directory
    "compilerDefines" : [], // an array of any -D arguments you want to pass to the 
	"compilerFlags" : ["-Werror"] // an array of any compiler flags 
}
```
The resulting dynamic library extension depends on the used compiler. 

The package.json would start empty in a new project, but adding packages using the sdk command it should look like the following:
```json
{
    "packages": [
        "boost-filesystem",
        "boost-system",
        "nlohmann-json"
    ]
}
```
Adding packages should be done using the vcbld add command which checks the availability of the package.
Notice that the versions are not included, that's because vcpkg builds packages from sources and doesn't keep previous versions of prebuilt binaries.
To check the version of the actually installed libraries used in your project, use the vcbld list command.
Furthermore, a project which uses vcbld can have it's missing dependencies restored using the restore sdk command.

Generating CMakeLists.txt files would include many of the variables in the vcbld.json file. Packages with known components with find_package support would use the find_package command in the CMakeLists.txt file. For the time being it includes Boost and Qt components. Other libraries would be located using the find_library command. Although vcbld doesn't support meta-object compilation, generating CMakeLists would allow meta-object compilation.



