#project(resulttypetests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(doctest)

include(warnings)
set(SOURCE_FILES
        TestMain.cpp
        Test_possible_combinations.cpp

        Test_hepler.cpp
        properties_tests.cpp
        Test_helper_success_error.cpp
        Test_into_exitcode.cpp Test_stream.cpp)
if (NOT CPPRESULTTYPE_WITH_BOOST_VARIANT)
list(APPEND SOURCE_FILES
        Test_tuple_holding_result.cpp
        Test_with_variants.cpp
        )
endif ()
add_executable(resulttypetests ${SOURCE_FILES})


target_link_libraries(resulttypetests PRIVATE cppresulttype_result doctest)

target_compile_options(resulttypetests PUBLIC ${WARNINGOPTS})


add_test(
        NAME resulttypetests
        COMMAND resulttypetests
)


add_subdirectory(typetraits)