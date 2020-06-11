#project(resulttypetests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EXTERN_DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
include(catch2)
include(warnings)
set(SOURCE_FILES
        TestMain.cpp
        Test_possible_combinations.cpp
        Test_with_variants.cpp
        properties_tests.cpp
        Test_tuple_holding_result.cpp
        Test_hepler.cpp
        )

add_executable(resulttypetests ${SOURCE_FILES})
target_include_directories(resulttypetests
                            PRIVATE ${CMAKE_SOURCE_DIR}/include
                            SYSTEM ${EXTERN_CATCH_HEADER_DIR})
add_test(
        NAME resulttypetests
        COMMAND resulttypetests
)
