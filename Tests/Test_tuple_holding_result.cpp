//
// Created by marcel on 2/29/20.
//

#include <type_traits>
#include "catch.hpp"
#include "result_type.h"
#include <tuple>

enum class Error {
    Fail1, NotOdd
};

TEST_CASE("Given a tuple with at no member of type Result<S,E>, when passing this type to ContainsResult, then ::value is false")
{
    using MyType = std::tuple<float,int>;

    CHECK_FALSE(ResultType::detail::is_tuple_with_result<MyType>::value);
}

TEST_CASE("Given a tuple with at least one member of type Result<S,E>, when passing this type to ContainsResult, then ::value is true")
{
    using MyType = std::tuple<int, ResultType::Result<float, Error >, float>;

    CHECK(ResultType::detail::is_tuple_with_result<MyType>::value);
}

TEST_CASE("Given a type that is not a tuple, when passing this type to ContainsResult, then ::value is false")
{
    using MyType = std::tuple<float>;

    CHECK_FALSE(ResultType::detail::is_tuple_with_result<MyType>::value);
}

TEST_CASE("Given a type which is not Result<S,E>, when passing this type to success_type, then ::type is type")
{
    CHECK(std::is_same_v<ResultType::detail::success_type<float>::type, float>);
}

TEST_CASE("Given a type which is Result<S,E>, when passing this type to success_type, then ::type is S")
{
    CHECK(std::is_same_v<ResultType::detail::success_type<ResultType::Result<float, Error >>::type, float>);
}

TEST_CASE("Given a tuple<Result<S1,E>, Result<S2,E>>, when its members hold success, then it gets converted to  Result<std::tuple<S1, S2>,E>")
{
    auto  input = std::make_tuple(ResultType::Result<float, Error >(3.2f), 3, ResultType::Result<std::string, Error >("lol"));
    auto const actual = ResultType::helper::success_tuple_or_err(std::move(input));
    auto const expected = ResultType::Result<std::tuple<float, int, std::string>, Error >{std::make_tuple(3.2f, 3, std::string("lol"))};

    REQUIRE(actual == expected);
}

TEST_CASE("Given a tuple with Result types and other types, when one of its Result type members holds an error, then it gets converted to  Result<std::tuple<S1, S2>,E> holding this error")
{
    using ResultType::operator|;
    auto  input = []{return std::make_tuple(3.5f, 10, ResultType::Result<float, Error >(3.2f), std::string{}, ResultType::Result<std::string, Error >(Error::NotOdd));};
    // this will not work: input() | ResultType::success_tuple_or_err;
    // the callee needs to be wrapped with a lamda expression
    auto  success_tuple_or_err = [](auto&& item){return ResultType::helper::success_tuple_or_err(std::forward<decltype(item)>(item));};
    auto  actual = input() | success_tuple_or_err;

    using ActualType   = decltype(actual);
    using InputType    = std::tuple<float, int, ResultType::Result<float, Error >, std::string, ResultType::Result<std::string, Error >>;
    using ExpectedType = ResultType::Result<std::tuple<float,int,float,std::string, std::string>, Error>;

    CHECK(std::is_same_v<InputType, decltype(input())>);
    CHECK(std::is_same_v<ActualType, ExpectedType>);

    CHECK(ResultType::IsError(actual));
}
