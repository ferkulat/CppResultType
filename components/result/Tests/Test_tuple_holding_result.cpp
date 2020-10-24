//
// Created by marcel on 2/29/20.
//

#include <type_traits>
#include <catch2/catch.hpp>
#include "result_type/result.hpp"
#include <tuple>

enum class Error {
    Fail1, NotOdd
};

TEST_CASE("Given a tuple with at no member of type Result<S,E>, when passing this type to ContainsResult, then ::value is false")
{
    using MyType = std::tuple<float,int>;

    CHECK_FALSE(result_type::detail::is_tuple_with_result<MyType>::value);
}

TEST_CASE("Given a tuple with at least one member of type Result<S,E>, when passing this type to ContainsResult, then ::value is true")
{
    using MyType = std::tuple<int, result_type::Result<float, Error >, float>;

    CHECK(result_type::detail::is_tuple_with_result<MyType>::value);
}

TEST_CASE("Given a type that is not a tuple, when passing this type to ContainsResult, then ::value is false")
{
    using MyType = std::tuple<float>;

    CHECK_FALSE(result_type::detail::is_tuple_with_result<MyType>::value);
}

TEST_CASE("Given a type which is not Result<S,E>, when passing this type to success_type, then ::type is type")
{
    CHECK(std::is_same_v<result_type::detail::success_type<float>::type, float>);
}

TEST_CASE("Given a type which is Result<S,E>, when passing this type to success_type, then ::type is S")
{
    CHECK(std::is_same_v<result_type::detail::success_type<result_type::Result<float, Error >>::type, float>);
}

TEST_CASE("Given a tuple<Result<S1,E>, Result<S2,E>>, when its members hold success, then it gets converted to  Result<std::tuple<S1, S2>,E>")
{
    auto  input = std::make_tuple(result_type::Result<float, Error >(3.2f), 3, result_type::Result<std::string, Error >("lol"));
    auto const actual = result_type::helper::success_tuple_or_err(std::move(input));
    auto const expected = result_type::Result<std::tuple<float, int, std::string>, Error >{std::make_tuple(3.2f, 3, std::string("lol"))};

    REQUIRE(actual == expected);
}

TEST_CASE("Given a tuple with Result types and other types, when one of its Result type members holds an error, then it gets converted to  Result<std::tuple<S1, S2>,E> holding this error")
{
    using result_type::operator|;
    auto  input = []{return std::make_tuple(3.5f, 10, result_type::Result<float, Error >(3.2f), std::string{}, result_type::Result<std::string, Error >(Error::NotOdd));};
    // this will not work: input() | result_type::success_tuple_or_err;
    // the callee needs to be wrapped with a lamda expression
    auto  success_tuple_or_err = [](auto&& item){return result_type::helper::success_tuple_or_err(std::forward<decltype(item)>(item));};
    auto  actual = input() | success_tuple_or_err;

    using ActualType   = decltype(actual);
    using InputType    = std::tuple<float, int, result_type::Result<float, Error >, std::string, result_type::Result<std::string, Error >>;
    using ExpectedType = result_type::Result<std::tuple<float,int,float,std::string, std::string>, Error>;

    CHECK(std::is_same_v<InputType, decltype(input())>);
    CHECK(std::is_same_v<ActualType, ExpectedType>);

    CHECK(result_type::IsError(actual));
}
