# vcbld
A command-line software development kit and a minimalist build system for C and C++ that works with vcpkg. For more information regarding the vcbld-gui, check the dedicated section and the tutorial video. vcbld has a minimal build system which works for small single hierarchy projects and allows for faster prototyping, however it has support for cmake and can be used to generate CMakeLists.txt files which can be seperately run using cmake and used to generate other build systems.
The vcbld executable is about 500kb large, and depends on the presence of vcpkg and cmake. Of course you would also need a C/C++ compiler such as gcc or clang.

```
SDK commands:
    setup                           Creates a conf.json in the project directory.
    new        [app|dylib|statlib]  Starts a new project.
    restore                         Restore missing dependencies in a project.
    build      [debug|release]      Builds the project.
    clean      [debug|release]      Clean build output(s).
    run        [debug|release]      Executes the project.
    includes                        Generates includePath.json file.
    gen                             Generates CMakeLists.txt file.
    list                            Shows a list of packages added to the project.
    all                             Shows a list of installed vcpkg packages.
    find       [package name]       Search within installed vcpkg packages.
    search     [package name]       Search vcpkg ports for packages to install.
    add        [package names]      Add package to the project.
    remove     [package names]      Remove package from the project.
    install    [package names]      Install packages using vcpkg and add project.
    uninstall  [package names]      Uninstall packages using vcpkg and remove from project.
    vcpkg      [vcpkg command]      Runs vcpkg commands.
    cmake      [config + args]      Runs cmake in the output directory using the vcpkg toolchain.
    make       [make arguments]     Runs make in the output directory.
    help                            Show help.

    --version                       Current vcbld version in use.
```

#### Some shortcut flags:
You can use the -g flag to signify debug, and the -r flag to signify release.
```
$ vcbld build -g
$ vcbld build -r
```

## Getting vcbld:
You can download a prebuilt vcbld executable [here](https://github.com/MoAlyousef/vcbld/releases). Currently there are prebuilt binaries for macOS X, debian linux and win32. Otherwise, you can build from source.

## Building vcbld:
vcbld has a single external dependency, namely Nlohmann-json which is a header-only library and is included in this git repository. To build vcbld, create a directory called vcbld. Access that directory using the command line. Clone this repository and build using cmake. The command line commands would look something like this:
```
$ git clone https://github.com/MoAlyousef/vcbld.git
$ cd vcbld && mkdir bin && cd bin
$ cmake .. && make
```
Notice that vcbld needs cmake version 3.10 or higher to build. vcpkg ships with an updated version of cmake that you can use to build vcbld, which can be found in [vcpkg-root]/downloads/tools/cmake*/cmake*/bin/cmake.
The built vcbld executable can be found in the release directory. Adding the vcbld executable to the vcpkg directory ensures it finds vcpkg if vcpkg isn't in a typical location as in your PATH, HOME or Program Files.
Note that vcbld doesn't automatically add itself to your PATH enviroment variable. 
However, you can add it to the PATH enviroment variable in linux using:
 ```
 $ echo 'export PATH=$PATH:/path/to/vcbld/directory' >> ~/.bashrc 
 ```
 or for mac osx:
 ```
 $ echo 'export PATH=$PATH:/path/to/vcbld/directory' >> ~/.bash_profile 
 ```
 
Or after building from source, by passing the make install command:
```
$ make install
``` 

Now you're ready to go!
Remember you can always access the help menu using:
```
$ vcbld help
```

## New application project:
Create a directory for your project and run: 
```
$ vcbld new app
```
This will generate a project with the following structure:
```
|
|__vcbld.json
|
|__conf.json
|
|__package.json
|
|__src
|
|__include
|
|__lib
|    |
|    |__debug
|    |
|    |__release
|
|__bin	
     |
     |__debug
     |
     |__release
```
The conf.json file contains several default variables depending on the operating system. Thus it is not supposed to be pushed to a version control repository like github and should be added to your gitignore (if you were using git). The default variables can also be changed if need be. A conf.json file will look like the following:
```json
{
	"cCompilerPath" : "/usr/bin/gcc-8",
	"cppCompilerPath" : "/usr/bin/g++-8",
	"vcpkgPath" : "/Users/mohammedalyousef/vcpkg/vcpkg",
	"architecture" : "x64-osx",
	"cmakePath" : "/Users/mohammedalyousef/vcpkg/downloads/tools/cmake-3.11.4-osx/cmake-3.11.4-Darwin-x86_64/CMake.app/Contents/bin/cmake",
        "makePath" : "/usr/bin/make",
        "archiverPath" : "/usr/bin/ar"
}
```
Notice that it contains absolute paths!
The first 2 variables are the paths of the C and C++ compilers. vcbld lets you choose the default compiler for your project when you start a new project or when you set one up.
The vcpkgPath variable is the path to the vcpkg executable you wish to choose for your project.

The architecture depends on the host operating system. You can check the vcpkg triplet documentation [here.](https://github.com/Microsoft/vcpkg/blob/master/docs/users/triplets.md)

Although vcbld doesn't support cross-compilation on the same machine, vcbld projects can be cross-compiled on different operating systems.
cmakePath and makePath are your installation locations of cmake and make. The archiverPath is the path of the archiver used to build static libraries. vcbld checks the presence of an installed cmake in your vcpkg/downloads/tools directory which it would choose by default, otherwise, it specifies the cmake executable in your PATH.
vcbld supports building using cmake and make within the executable. It can also generate CMakeLists.txt files which can be seperately run using cmake and used to generate other build systems.

A new project will also have a vcbld.json and package.json files in the project directory.
A vcbld.json would look like the following (without the comments):
```
{
	"projectName" : "vcbld", // Name of the project.
	"version" : "0.1.0",
	"language" : "c++", // notice the lower case. camelCase is followed in vcbld JSON files.
	"standard" : "17",
	"binaryType" : "app", // or "staticLibrary" or "dynamicLibrary"
	"binaryName" : "vcbld", // Name of the resulting binary, use a "lib" prefix if it were a library.
    	"sourceDirectory" : "./src", // Name of your source directory
    	"includeDirectory" : "./include", // Name of your include directory, can be left blank.
	"libDirectory" : "", // Name of your lib directory, can be left blank.
    	"outputDirectory" : "./bin", // Name of your output directory, and it should be provided!
    	"compilerDefines" : [], // an array of any -D arguments you want to pass to the compiler
	"compilerFlags" : ["-Weverything"], // an array of any compiler flags 
    	"linkerFlags" : ["-lstdc++fs"] // an array of any linker flags 
}
```
The resulting dynamic library extension depends on the used compiler. libname.so for gcc and libname.dylib for clang.

## Adding and removing packages:
The package.json would start empty in a new project, but adding packages can be done using the sdk command:
```
$ vcbld add boost-filesystem boost-system nlohmann-json
```
The resulting package.json file should look like the following:
```json
{
    "packages": [
        "boost-filesystem",
        "boost-system",
        "nlohmann-json"
    ]
}
```
The add command checks the availability of the package within your vcpkg directory.
Notice that the versions are not included, that's because vcpkg builds packages from source and doesn't keep previous versions of prebuilt binaries. To check the version of the actually installed libraries used in your project, use the vcbld list command.
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

## Building using vcbld:
You can use the vcbld build system using the command:
```
$ vcbld build [debug or release]
```
Then you can run the executable using the run command:
```
$ vcbld run [debug or release]
```
Notice that if the build type isn't given, the default would be a debug build and debug run. vcbld keeps object files in the output directory alongside the built binary since certain licenses require you to supply your object files along with the binary. It also copies dynamically linked libraries into the output directory which are required to run your binary in other systems.

## Generating CMakeLists and includePath files:
The sdk also offers the possiblity to generate and includePath.json file which can be used to get intellisense if you're using an editor like visual studio code or atom.

Generating CMakeLists.txt files can be done using the following command:
```
$ vcbld gen
```
and it would include many of the variables in the vcbld.json file. Packages with known components and with find_package support would use the find_package command in the CMakeLists.txt file. For the time being it includes Boost and Qt components. Other libraries would be located using the find_library command since vcpkg doesn't guarantee find_package support for all packages. And although vcbld doesn't support meta-object compilation, generating CMakeLists would allow meta-object compilation.

CMake can be run using vcbld which would run within the output directory and use the vcpkg toolchain by default. You can then run make (on macOS X and linux) similarly to build the binary. You can also then run the run command on the generated executable.
```
$ vcbld cmake
$ vcbld make
$ vcbld run
```
Also you can supply arguments to your cmake command like the following:
```
$ vcbld cmake release
$ vcbld make
$ vcbld run release
```
Or to generate a visual studio project for example, you can run:
```
$ vcbld cmake -G "Visual Studio 15 2017 Win64"
```

Alternatively, you can run cmake manually on the generated CMakeLists.txt file, in which case you would need to use the vcpkg toolchain definition which should point to your vcpkg.cmake script. For more information, check the vcpkg [readme](https://github.com/Microsoft/vcpkg) or documentation:

Namely if vcpkg were installed in your home directory, the definition would like -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake 
(the tilde points to the home directory in posix systems). As such, for creating an out of source build:
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 15 2017 Win64" ..
```
Which would generate a visual studio project file, or to generate makefiles (which is the default output on linux and macOS X): 
```
$ cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Unix Makefiles" ..
```

For a full list of available generators, see [here.](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)

## vcpkg integration:
Additionally you can run vcpkg commands using vcbld, this is useful if you don't have vcpkg in your PATH, or if you're using several instances of vcpkg for different projects. You need to check, however, the path of the vcpkg instance you're using in the conf.json file.
```
$ vcbld vcpkg install boost-filesystem asio
```
You can also use it to apply user-wide integration using:
```
$ vcbld vcpkg integrate install
```

## vcbld-gui
Is still a work in progress. It's built using Qt5 widgets. It was tried on mac OS X and linux. You can download the prebuilt versions from the release page, or you can build from source. There are 2 versions of the gui, a standalone version which can run without having the prebuilt Qt libraries installed, and the stripped versions which run if you already have the Qt libraries installed. If you're having trouble running the stripped versions while having the Qt libraries installed, it probably means they're not installed as system libraries. Note that on kde flavors of linux, such as kubuntu or kde neon, the Qt libraries are already installed as system libraries, as such no further installation of Qt is needed. In that case, you can download the standalone versions or you can run the deploy kit which comes with Qt (macdeployqt or windeployqt). Note that the linuxdeployqt can be downloaded from the release page [here](https://github.com/probonopd/linuxdeployqt/releases/tag/continuous). Otherwise the deploy kits are usually located in the bin directory of your Qt installation. These are command line applications and are run as follows:
```
$ path/to/Qt/deploy/kit path/to/vcbld-gui
```
In my machine it would be:
```
$ ~/Qt/5.11.1/clang_64/bin/macdeployqt ~/Desktop/vcbld/bin/release/vcbld-gui.app
```
Using the linuxdeployqt, you can also generate an AppImage using the option -appimage.


Building the gui, you need to have Qt Creator or Kdevelop. Otherwise, you can download the libraries on debian flavors of linux using:
```
$ sudo apt-get install install qtbase5-dev
```
Also to build, you need to enable the WITH_GUI option in the top-level CMakeLists file by setting it to ON in the file itself or using the command line:
```
$ cd bin
$ cmake -DWITH_GUI=ON ..
$ make
```
The build output produces an app bundle along with a run script "vcbld-gui.command" on mac OS X, and an executable and a run script on linux. On windows it generates a batch file and an executable. Since Qt gui applications don't have access to your full PATH variable, running the application through the run script would be better. If you're downloading the script from the release page, you'd have to change the permission on the app using the following command:
```
$ chmod a+x path/to/vcbld-gui.command
```

## Tutorials:
Some video tutorials will be added to my youtube channel. 
This is the link to the first video:
https://www.youtube.com/watch?v=gE72ehRLQzE

Second video on building vcbld from source using cmake:
https://www.youtube.com/watch?v=hMTEm4Fzcgk

Third video on using external 3rd party libraries with vcbld:
https://www.youtube.com/watch?v=KBeY5pgsJQo

Fourth video on building vcbld using vcbld:
https://www.youtube.com/watch?v=zY-8IX6Lnsk

Fifth video on using 3rd party libraries - 2nd part:
https://www.youtube.com/watch?v=2qpDG8DyJnI

Sixth video and first video on the vcbld-gui:
https://www.youtube.com/watch?v=dsBqfPNM-w8

Seventh and second video on the usage of the gui:
https://www.youtube.com/watch?v=l5v3Bl8U-GI

Using vcbld and vcbld-gui in a windows environment:
https://www.youtube.com/watch?v=6lYCwM-UlNE

## Contributing:
All contributions are welcome!

For the moment, vcbld was tried on macOS X and linux. Development on windows is needed as well.

## License:
Code licensed under the [MIT License.](https://github.com/MoAlyousef/vcbld/blob/master/LICENSE)




