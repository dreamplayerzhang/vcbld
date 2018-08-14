# vcbld
A build system to work with vcpkg. It works for the moment on posix systems, i.e. linux and mac os x. It runs on windows if you're using MinGW or MSYS. It has a minimal build system which works for small single hierarchy projects which allows for faster prototyping, however it has support for cmake and can be used to generate CMakeLists.txt files which can be seperately run using cmake and used to generate other build systems.
The vcbld executable is about 600kb large, and depends on the presence of vcpkg and cmake. Of course you would also need a posix C/C++ compiler such as gcc, clang or mingGW.

```
SDK commands:
    setup                           Creates a conf.json in the project directory.
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
    cmake   [cmake arguments]       Runs cmake in the output directory using the vcpkg toolchain.
    make    [make arguments]        Runs make in the output directory.
    help                            Show help.

    --version                       Current vcbld version in use.
```

## Building vcbld:
vcbld has some external library dependencies, namely Boost::filesystem (which depends on Boost::system) and Nlohmann_json.
These can be installed using vcpkg using the following command:
```
$ vcpkg install boost-filesystem boost-system nlohmann-json
```
Create a directory called vcbld. Access that directory using the command line. Clone this repository and build using cmake. The command line commands would look something like this. 
```
$ git clone https://github.com/MoAlyousef/vcbld.git
$ cd vcbld
$ mkdir bin
$ cd bin
$ cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/vcpkg/scripts/buildsystems/vcpkg.cmake ..
$ make
```
The cmake command should use the CMAKE_TOOLCHAIN_FILE definition which should point to your vcpkg.cmake script. For more information, check the vcpkg [readme](https://github.com/Microsoft/vcpkg) or documentation:

Namely if vcpkg were installed in your home directory, it would be ~/vcpkg/scripts/buildsystems/vcpkg.cmake (the tilde points to the home directory in posix systems).

The built vcbld executable can be found in the release directory. vcbld doesn't automatically add itself to your PATH, however you can do so after building from source on mac os x and linux using:
```
$ make install
```
On windows you can add the vcbld binary directory to your path via accessing Control Panel -> System -> Advanced system settings -> Environment variables -> Edit System Variable (or New System Variable). There you can add the folder to your PATH.

Now you're ready to go!
Remember you can always access the help menu using:
```
$ vcbld help
```

## New application project:
Run: 
```
$ vcbld new app
```
This will generate a conf.json file in the directory of the executable. It contains several default variables depending on the operating system, which can also be changed if need be. A conf.json file will look like the following:
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
The conf.json is not supposed to be pushed to a version control repository like github since it's dependant on local variables and thus should be added to your gitignore (if you were using git).
The first 2 variables are the paths of the c and c++ compilers. The default compilers for mac os x is clang, while for linux it would be gcc and g++ respectively. On windows, the default compiler is the MinGW gcc.
The vcpkgDirectory variable is the parent directory of the vcpkg executable you wish to choose for your project. It defaults to the home path on linux and mac os x, while on windows it defaults to C:/Program Files/vcpkg.

The architecture depends on the host operating system. You can check the vcpkg triplet documentation [here.](https://github.com/Microsoft/vcpkg/blob/master/docs/users/triplets.md)

Although vcbld doesn't support cross-compilation on the same machine, vcbld projects can be cross-compiled on different operating systems.
cmakePath and makePath are your installation locations of cmake and make. vcbld supports building using cmake and make within the executable. It can also generate CMakeLists.txt files which can be seperately run using cmake and used to generate other build systems.
On windows, the default make file is the one supplied by the MinGW installation.

A new project will also have a vcbld.json and package.json files in project directory.
A vcbld.json would look like the following (without the comments):
```
{
	"projectName" : "myProj", // Name of the project.
	"version" : "0.1.0",
	"language" : "c++", 
	"standard" : "11", // The minimum supported standards of both of c++ and c.
	"binaryType" : "app", // or "staticLibrary" or "sharedLibrary"
	"binaryName" : "myApp", // Name of the resulting binary, use a "lib" prefix if it were a library.
    	"sourceDirectory" : "src", // Relative or absolute path of your source directory
    	"includeDirectory" : "include",
	"libsDirectory" : "libs",
    	"outputDirectory" : "bin", // Relative or absolute path to your output directory
    	"compilerDefines" : [], // an array of any -D arguments you want to pass to the 
	"compilerFlags" : ["-Werror"] // an array of any compiler flags 
}
```
The resulting dynamic library extension depends on the used compiler. libname.so for gcc and libname.dylib for clang.

## Adding and removing packages:
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
Adding packages should be done using the vcbld add command:
```
$ vcbld add boost-filesystem
```
which checks the availability of the package within your vcpkg directory.
Notice that the versions are not included, that's because vcpkg builds packages from sources and doesn't keep previous versions of prebuilt binaries. To check the version of the actually installed libraries used in your project, use the vcbld list command.
To check the available packages within the vcpkg directory, use:
```
$ vcbld list
```
Removing packages can be done with the remove <package> command:
```
$ vcbld remove nlohmann-json
```
	
You can also search within the available packages using:
```
$ vcbld search asio
```

If a project was built using vcbld, the first command to run would be:
```
$ vcbld setup
$ vcbld restore
```
Setup creates a conf.json file for the project. Restore installs missing packages via vcpkg.

You can use the vcbld build system using the command:
```
$ vcbld build [debug or release]
$ vcbld run [debug or release]
```
Then you can run the executable using the run command. Notice that if the build type isn't given, the default would be a debug build and debug run.

## Generating CMakeLists and includePath files
The sdk also offers the possiblity to generate and includePath.json file which can be used to get intellisense if you're using an editor like visual studio code or atom.

Generating CMakeLists.txt files can be done using the following command:
```
$ vcbld gen
```
and it would include many of the variables in the vcbld.json file. Packages with known components and with find_package support would use the find_package command in the CMakeLists.txt file. For the time being it includes Boost and Qt components. Other libraries would be located using the find_library command. And although vcbld doesn't support meta-object compilation, generating CMakeLists would allow meta-object compilation.

CMake can be run using vcbld which would run within the output directory and use the vcpkg toolchain by default. You can then run make (on mac os x and linux) similarly to build the binary. You can also then run the run command on the generated executable.
```
$ vcbld cmake
$ vcbld make
$ vcbld run
```
Also you can supply arguments to your cmake command like the following:
```
$ vcbld cmake -DCMAKE_BUILD_TYPE=Release
$ vcbld make
$ vcbld run release
```
Or to generate a visual studio project for example, you can run:
```
$ vcbld -G "Visual Studio 15 2017 Win64" ..
```

Alternatively, you can run cmake manually on the generated CMakeLists.txt file, in which case you would need to use the vcpkg toolchain. Creating an out of source build:
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 15 2017 Win64" ..
```
Which would generate a visual studio project file, or to generate makefiles (which is the default output on linux and mac os x): 
```
$ cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Unix Makefiles" ..
```

For a full list of available generators, see [here.](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

## vcpkg integration:
Additionally you can run vcpkg commands using vcbld, this is useful if you don't have vcpkg in your PATH, or if you're using several instances of vcpkg for different projects. You just need to check the path of the instance you're using in the conf.json file.
```
$ vcbld vcpkg install boost-filesystem nlohmann-json asio
```
You can also use it to apply user-wide integration using:
```
$ vcbld vcpkg integrate install
```

## License:
Code licensed under the [MIT License.](https://github.com/MoAlyousef/vcbld/blob/master/LICENSE)




