cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

include(FetchContent)

# ------------------------------------------------------------------------------
# A modern, C++-native, header-only, test framework for unit-tests,
# TDD and BDD - using C++11, C++14, C++17 and later
FetchContent_Declare(
  extern_catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.2
)

FetchContent_GetProperties(extern_catch2)
if(NOT extern_catch2_POPULATED)
    FetchContent_Populate(extern_catch2)
    add_subdirectory(${extern_catch2_SOURCE_DIR} ${extern_catch2_BINARY_DIR})
endif()
