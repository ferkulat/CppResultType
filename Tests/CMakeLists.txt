
set(SOURCE_FILES TestMain.cpp Test_1.cpp catch.hpp)

add_executable(tests ${SOURCE_FILES})
target_link_libraries (tests )
