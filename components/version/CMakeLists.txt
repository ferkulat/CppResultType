include(MunkeiVersionFromGit)
version_from_git(
  LOG       ON
  TIMESTAMP "%Y%m%d%H%M%S"
)


set(component cppresulttype_version)

set(${component}_export_name version)
set(${component}_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

set(${component}_INCLUDE_FILE ${${component}_INCLUDE_DIR}/result_type/version/version.hpp)
get_filename_component(${${component}_INCLUDE_DIR}_ABSOLUT ${${component}_INCLUDE_DIR} ABSOLUTE)
configure_file(${CMAKE_CURRENT_LIST_DIR}/version.hpp.in
  ${${component}_INCLUDE_FILE}
)

add_library(${component} INTERFACE)

add_library(${project}::${component}_export_name ALIAS ${component})

target_include_directories(${component}
  INTERFACE
  $<BUILD_INTERFACE:${${${component}_INCLUDE_DIR}_ABSOLUT}>
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
  DIRECTORY ${${component}_INCLUDE_DIR}
  COMPONENT ${component}
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)