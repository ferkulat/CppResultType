#include <doctest/doctest.h>
#include <result_type/result.hpp>
enum class Error{lol};

TEST_CASE("For Result<S,E>(S), isSuccess returns true"){
    REQUIRE(result_type::isSuccess(result_type::Result<int, Error>{2}));
}
TEST_CASE("For Result<S,E>(E), isSuccess returns true"){
    REQUIRE_FALSE(result_type::isSuccess(result_type::Result<int, Error>{Error{}}));
}
TEST_CASE("For Result<S,E>(E), isSuccess returns true"){
    REQUIRE(result_type::isSuccess(int{2}));
}

TEST_CASE("For Result<S,E>(S), isError returns false"){
            REQUIRE_FALSE(result_type::isError(result_type::Result<int, Error>{2}));
}
TEST_CASE("For Result<S,E>(E), isError returns true"){
            REQUIRE(result_type::isError(result_type::Result<int, Error>{Error{}}));
}
TEST_CASE("For Result<S,E>(E), isError returns true"){
            REQUIRE_FALSE(result_type::isError(int{2}));
}
