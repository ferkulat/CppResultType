#include <doctest/doctest.h>

#include <result_type/pipeoperator.hpp>

TEST_CASE("pipe a non const lvalue to a function accepting a non const reference, should compile"){
    auto fun = [](double& d){return d*2;};
    auto input = double{3.0};
    using result_type::operator|;
    auto const actual = input | fun;
    REQUIRE(actual == 6.0);
}
