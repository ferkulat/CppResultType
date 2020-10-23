#project(resulttypetests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

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


target_link_libraries(resulttypetests PRIVATE cppresulttype_resulttype Catch2::Catch2)

target_compile_options(resulttypetests PUBLIC ${WARNINGOPTS})


add_test(
        NAME resulttypetests
        COMMAND resulttypetests
)
