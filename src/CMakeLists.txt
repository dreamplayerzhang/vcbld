set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS filesystem system REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "args.cpp" "builder.cpp" "help.cpp" "conf.cpp" "pkg.cpp" "init.cpp" "gen.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ~/vcpkg/installed/x64-osx/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem Boost::system)
