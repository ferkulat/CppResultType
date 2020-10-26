#include <doctest/doctest.h>
#include <result_type/result.hpp>
enum class Error{lol};

TEST_CASE("For Result<S,E>(S), IsSuccess returns true"){
    REQUIRE(result_type::IsSuccess(result_type::Result<int, Error>{2}));
}
TEST_CASE("For Result<S,E>(E), IsSuccess returns true"){
    REQUIRE_FALSE(result_type::IsSuccess(result_type::Result<int, Error>{Error{}}));
}
TEST_CASE("For Result<S,E>(E), IsSuccess returns true"){
    REQUIRE(result_type::IsSuccess(int{2}));
}

TEST_CASE("For Result<S,E>(S), IsError returns false"){
            REQUIRE_FALSE(result_type::IsError(result_type::Result<int, Error>{2}));
}
TEST_CASE("For Result<S,E>(E), IsError returns true"){
            REQUIRE(result_type::IsError(result_type::Result<int, Error>{Error{}}));
}
TEST_CASE("For Result<S,E>(E), IsError returns true"){
            REQUIRE_FALSE(result_type::IsError(int{2}));
}
