#include <doctest/doctest.h>
#include <result_type/helper/into_exitcode.hpp>
#include <result_type/result.hpp>
#include <result_type/pipeoperator.hpp>
using result_type::operator|;

enum class Error{lol};
TEST_CASE("passing Result<Success,Error>(Sussess()) to intoExitCode, returns 0"){
    auto const actual   = result_type::Result<int , Error>(2) | result_type::helper::intoExitCode{};
    auto const expected = 0;

    REQUIRE( actual == expected );
}

TEST_CASE("passing Result<Success,Error>(Error()) to intoExitCode, returns -1"){
    auto const actual   = result_type::Result<int , Error>(Error{}) | result_type::helper::intoExitCode{};
    auto const expected = -1;

    REQUIRE( actual == expected );
}

TEST_CASE("passing int to intoExitCode, returns this int value"){
    auto const actual   = 3 | result_type::helper::intoExitCode{};
    auto const expected = 3;

    REQUIRE( actual == expected );
}
