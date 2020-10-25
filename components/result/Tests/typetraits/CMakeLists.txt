include(doctest)
include(warnings)
set(SOURCE_FILES main.cpp Test_type_traits.cpp)

add_executable(typetraitstests  ${SOURCE_FILES})
target_link_libraries(typetraitstests PRIVATE cppresulttype_result doctest)
target_compile_options(typetraitstests PUBLIC ${WARNINGOPTS})
add_test(
        NAME typetraitstests
        COMMAND typetraitstests
)
