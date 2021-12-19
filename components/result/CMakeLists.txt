cmake_minimum_required(VERSION 3.11)

set(component cppresulttype_result)
set(${component}_export_name result)

set(${component}_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
add_library(${component}  INTERFACE)

add_library(${project}::${${component}_export_name} ALIAS ${component})

target_include_directories(${component}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )

install(TARGETS ${component} EXPORT ${component}-targets
        COMPONENT ${component}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(EXPORT ${component}-targets
        FILE "${project}-${component}-targets.cmake"
        NAMESPACE ${project}::
        DESTINATION lib/cmake/${project}
        COMPONENT ${component}
        )

# This seems like a kludge, but it does place the file in the correct location
# on my machine (Ubuntu 18.04). Idea taken from Poco
configure_file("${component}-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${project}-${component}-config.cmake"
        @ONLY
        )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${project}-${component}-config-version.cmake"
        VERSION ${version}
        COMPATIBILITY AnyNewerVersion
)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${project}-${component}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${project}-${component}-config-version.cmake"
        DESTINATION lib/cmake/${project}
        COMPONENT ${component}
)

#install(
#        DIRECTORY components/resulttype/include
#        COMPONENT ${component}
#        DESTINATION ${CMAKE_INSTALL_PREFIX}
#)

#set(CMAKE_VERBOSE_MAKEFILE ON)
add_subdirectory(Tests)
