message("CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}")
message("CMAKE_CXX_PLATFORM_ID ${CMAKE_CXX_PLATFORM_ID}")
message("CMAKE_CXX_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION}")


try_compile(CPPRESULTTYPE_CAN_USE_STD_APPLY
        "${CMAKE_CURRENT_BINARY_DIR}/test_for_std_apply"
        "${CMAKE_CURRENT_LIST_DIR}/../feature_tests/test_for_std_apply"
        test_for_std_apply_app
        )

try_compile(CPPRESULTTYPE_CAN_USE_OVERLOAD_PATTERN
        "${CMAKE_CURRENT_BINARY_DIR}/can_use_overload_pattern"
        "${CMAKE_CURRENT_LIST_DIR}/../feature_tests/can_use_overload_pattern"
        can_use_overload_pattern
        )

try_compile(CPPRESULTTYPE_CAN_USE_CONSTEXPR_IF
        "${CMAKE_CURRENT_BINARY_DIR}/can_use_constexpr_if"
        "${CMAKE_CURRENT_LIST_DIR}/../feature_tests/can_use_constexpr_if"
        can_use_constexpr_if
        )

try_compile(CPPRESULTTYPE_CAN_USE_STD_OPTIONAL
        "${CMAKE_CURRENT_BINARY_DIR}/can_use_std_optional"
        "${CMAKE_CURRENT_LIST_DIR}/../feature_tests/can_use_std_optional"
        can_use_std_optional
        )

try_compile(CPPRESULTTYPE_CAN_USE_STD_VARIANT
        "${CMAKE_CURRENT_BINARY_DIR}/can_use_std_variant"
        "${CMAKE_CURRENT_LIST_DIR}/../feature_tests/can_use_std_variant"
        can_use_std_variant
        )

message("Can use 'std::apply'    : " ${CPPRESULTTYPE_CAN_USE_STD_APPLY} )
message("Can use overload pattern: " ${CPPRESULTTYPE_CAN_USE_OVERLOAD_PATTERN} )
message("Can use 'if constexpr()': " ${CPPRESULTTYPE_CAN_USE_CONSTEXPR_IF} )
message("Can use 'std::optional' : " ${CPPRESULTTYPE_CAN_USE_STD_OPTIONAL} )
message("Can use 'std::variant'  : " ${CPPRESULTTYPE_CAN_USE_STD_VARIANT} )

if(CPPRESULTTYPE_CAN_USE_STD_VARIANT)
    set(DEFAULT_BOOST_VARIANT_OPTION OFF)
else()
    set(DEFAULT_BOOST_VARIANT_OPTION ON)
endif()

if(CPPRESULTTYPE_CAN_USE_STD_OPTIONAL)
    set(DEFAULT_BOOST_OPTIONAL_OPTION OFF)
else()
    set(DEFAULT_BOOST_OPTIONAL_OPTION ON)
endif()

if(CPPRESULTTYPE_CAN_USE_CONSTEXPR_IF)
    set(DEFAULT_CPP14_PIPE_OPTION OFF)
else()
    set(DEFAULT_CPP14_PIPE_OPTION ON)
endif()

if(CPPRESULTTYPE_CAN_USE_STD_APPLY)
    set(DEFAULT_STD_APPLY_OPTION ON)
else()
    set(DEFAULT_STD_APPLY_OPTION OFF)
endif()

option(CPPRESULTTYPE_WITH_BOOST_VARIANT "use boost::variant" ${DEFAULT_BOOST_VARIANT_OPTION})

option(CPPRESULTTYPE_WITH_BOOST_OPTIONAL "use boost::optional" ${DEFAULT_BOOST_OPTIONAL_OPTION})

option(CPPRESULTTYPE_WITH_STD_APPLY "use std::apply" ${DEFAULT_STD_APPLY_OPTION} )

message("setting option CPPRESULTTYPE_WITH_BOOST_VARIANT :" ${CPPRESULTTYPE_WITH_BOOST_VARIANT})
message("setting option CPPRESULTTYPE_WITH_BOOST_OPTIONAL:" ${CPPRESULTTYPE_WITH_BOOST_OPTIONAL})
message("setting option CPPRESULTTYPE_WITH_STD_APPLY     :" ${CPPRESULTTYPE_CAN_USE_STD_APPLY})

if (CPPRESULTTYPE_WITH_BOOST_VARIANT)
    add_definitions(-DCPPRESULTTYPE_WITH_BOOST_VARIANT)
endif ()

if (CPPRESULTTYPE_WITH_BOOST_OPTIONAL)
    add_definitions(-DCPPRESULTTYPE_WITH_BOOST_OPTIONAL)
endif ()

if (CPPRESULTTYPE_WITH_STD_APPLY)
    add_definitions(-DCPPRESULTTYPE_WITH_STD_APPLY)
endif ()
