cmake_minimum_required(VERSION 3.11)
set(component cppresulttype_resulttype)


add_library(${component}  INTERFACE)

add_library(${project}::${component} ALIAS ${component})

target_include_directories(${component}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

install(TARGETS ${component} EXPORT ${component}-targets
        COMPONENT ${component}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION components/${component}/include
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
        "${CMAKE_BINARY_DIR}/${project}-${component}-config.cmake"
        @ONLY
        )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/${project}-${component}-config-version.cmake"
        VERSION ${version}
        COMPATIBILITY AnyNewerVersion
)

install(
        FILES
        "${CMAKE_BINARY_DIR}/${project}-${component}-config.cmake"
        "${CMAKE_BINARY_DIR}/${project}-${component}-config-version.cmake"
        DESTINATION lib/cmake/${project}
        COMPONENT ${component}
)

install(
        DIRECTORY components/resulttype/include
        COMPONENT ${component}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)

#set(CMAKE_VERBOSE_MAKEFILE ON)
add_subdirectory(Tests)
