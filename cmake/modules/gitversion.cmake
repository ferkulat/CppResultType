
include(MunkeiVersionFromGit)
version_from_git(
        LOG       ON
        TIMESTAMP "%Y%m%d%H%M%S"
)

configure_file(${CMAKE_CURRENT_LIST_DIR}/version.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.cmake)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")
message("VERSIONFILE: " ${version_file})

