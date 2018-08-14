set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "-Weverything")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

find_package(Boost COMPONENTS filesystem system REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "args.cpp" "builder.cpp" "help.cpp" "conf.cpp" "init.cpp" "gen.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem Boost::system)

install(
	TARGETS ${PROJECT_NAME}
	CONFIGURATIONS Release
    RUNTIME DESTINATION bin
)

if(WIN32)
include(CPack REQUIRED)
include(CPackIFW REQUIRED)

cpack_add_component(${PROJECT_NAME}.exe
    DISPLAY_NAME "vcbld"
    DESCRIPTION "A build system to work with vcpkg")
cpack_ifw_configure_component(${PROJECT_NAME}.app
    VERSION "0.1.0")
cpack_ifw_add_repository(myrepo
    URL "https://github.com/MoAlyousef/vcbld"
	DISPLAY_NAME "vcbld repository.")
endif()