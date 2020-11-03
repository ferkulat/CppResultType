#include <doctest/doctest.h>
#include <result_type/pipeoperator.hpp>
enum class ErrorType {lol};
TEST_CASE("pipe a non const lvalue to a function accepting a non const reference, should compile"){
    auto fun = [](double& d){return d*2;};
    auto input = double{3.0};
    using result_type::operator|;
    auto const actual = input | fun;
    REQUIRE(actual == 6.0);
}

TEST_CASE("pipe a non const lvalue Result<Optional<S>,E> to a function accepting a non const reference of S, should compile"){
    auto fun = [](double& d){return d*2;};
    auto input = result_type::Result<result_type::Optional<double>, ErrorType>{double{3.0}};
    using result_type::operator|;
    auto const actual = input | fun;
    REQUIRE(actual.Success().value() == 6.0);
}

TEST_CASE("pipe a non const lvalue Optional<T> to a function accepting a non const reference of T, should compile"){
    auto fun = [](double& d){return d*2;};
    auto input = result_type::Optional<double>{double{3.0}};
    using result_type::operator|;
    auto const actual = input | fun;
    REQUIRE(actual);
}

TEST_CASE("pipe a non const lvalue Result<S,E> to a function accepting a non const reference of S, should compile"){
    auto fun = [](double& d){return d*2;};
    auto input = result_type::Result<double, ErrorType>{double{3.0}};
    using result_type::operator|;
    auto const actual = input | fun;
    REQUIRE(actual.Success() == 6.0);
}
