cmake_minimum_required(VERSION 3.11)
project(result_type)


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set (BUILD_TESTING false)
include(CTest)
enable_testing()
add_subdirectory(Tests)
