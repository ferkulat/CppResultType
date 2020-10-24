if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    set( WARNINGOPTS -Wall -Wextra -Wpedantic -Warray-bounds -Wshadow -Wconversion -Wsign-conversion -Wparentheses -Werror -ftemplate-backtrace-limit=0  -Wno-attributes -Wno-unused-parameter)
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    set( WARNINGOPTS  /W4 /WX )
endif()
