cmake_minimum_required(VERSION 3.11)
set (project cppresulttype)
project(${project})


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(gitversion)
include(version)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set (BUILD_TESTING false)
include(CTest)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(components)

include ( CMakePackageConfigHelpers )
# Destination of the installed config files (relative path):
set ( ${PROJECT_NAME}_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" )
set ( CMAKE_CONFIG_DEST "share/cmake/Modules" )
configure_package_config_file(${project}Config.cmake.in
        "${CMAKE_BINARY_DIR}/${project}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_CONFIG_DEST}"
        )
# This file is included in our template:
export ( TARGETS FILE "${CMAKE_BINARY_DIR}/${project}Targets.cmake" )

export ( PACKAGE ${PROJECT_NAME} )

