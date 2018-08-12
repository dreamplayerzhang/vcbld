set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/cmake_bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "-Weverything")

find_package(Boost COMPONENTS filesystem system REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "args.cpp" "builder.cpp" "help.cpp" "conf.cpp" "init.cpp" "gen.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem Boost::system)
