cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

include(FetchContent)

# ------------------------------------------------------------------------------
# A modern, C++-native, header-only, test framework for unit-tests,
# TDD and BDD - using C++11, C++14, C++17 and later
FetchContent_Declare(
  extern_doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        2.4.0
)

FetchContent_GetProperties(extern_doctest)
if(NOT extern_doctest_POPULATED)
    FetchContent_Populate(extern_doctest)
    add_subdirectory(${extern_doctest_SOURCE_DIR} ${extern_doctest_BINARY_DIR})
endif()
