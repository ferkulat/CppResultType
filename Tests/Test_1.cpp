//
// Created by marcel on 3/30/17.
//
#include "catch.hpp"
#include "result_type.h"

using ResultType::operator|;
using ResultType::Result;
using ResultType::IsSuccess;
using ResultType::IsError;
constexpr auto multiplyBy(int factor) {
    return [factor](int val){
        return val * factor;
    };
}
enum class Error{Fail1, NotOdd};
enum class Error1{Fail1, NotOdd};

constexpr auto OddsToStr(){
    using ReturnType = Result<std::string, Error>;
    return [](Result<int, Error>&& rei){
        if (IsSuccess(rei)){
            return (rei.RefSuccess()%2)? ReturnType(std::to_string(rei.RefSuccess()))
                                       : ReturnType(Error::NotOdd);
        }
        return ReturnType(std::move(rei).Error());
    };
}

auto ReturnNothing (int&o){
    return [&o](int i){
        o=i*3;
    };
};

TEST_CASE("(int | F(int)->int) -> int")
{
    constexpr auto actual = 2 | multiplyBy(2);
    REQUIRE( 4 == actual );
}

TEST_CASE("(Result<int, Error>(int) | F(int)->int) -> Result<int, Error>(int)")
{
    constexpr auto actual = Result<int, Error>(2) | multiplyBy(3);
    REQUIRE( IsSuccess(actual) );
    REQUIRE(actual.CRefSuccess() == 6);
}

TEST_CASE("(Result<int, Error>(Error) | F(int)->int) -> Result<int, Error>(Error)")
{
constexpr auto actual = Result<int, Error>(Error::Fail1) | multiplyBy(3);
REQUIRE( IsError(actual) );
REQUIRE(actual.CRefError() == Error::Fail1);
}

TEST_CASE(R"((Result<int, Error>(3) | F(Result<int, Error>)->Result<std::string, Error>) -> Result<std::string, Error>("3"))")
{
    auto actual = Result<int, Error>(3) | OddsToStr();
    REQUIRE( IsSuccess(actual) );
    REQUIRE(actual.CRefSuccess() == "3");
}

TEST_CASE(R"((Result<int, Error>(Error::Fail1) | F(Result<int, Error>)->Result<std::string, Error>) -> Result<std::string, Error>(Error::Fail1))")
{
    auto actual = Result<int, Error>(Error::Fail1) | OddsToStr();
    REQUIRE( IsError(actual) );
    REQUIRE(actual.CRefError() == Error::Fail1);
}

TEST_CASE(R"((Result<int, Error>(int) | F(Result<int, Error>)->Result<std::string, Error>) -> Result<std::string, Error>(Error::NotOdd))")
{
    auto actual = Result<int, Error>(2) | OddsToStr();
    REQUIRE( IsError(actual) );
    REQUIRE(actual.CRefError() == Error::NotOdd);
}

TEST_CASE(R"(int | F(int)-void )->void -> void)")
{
    int o{};
    2 | ReturnNothing(o);
    REQUIRE(o == 6);
}

TEST_CASE(R"(Result<int, Error>(int) | F(int)-void )->void -> void)")
{
    int o{};
    Result<int, Error>(3) | ReturnNothing(o);
    REQUIRE(o == 9);
}

TEST_CASE(R"(Result<T, EX>() | F(Result<T, EY>)->? ) should not compile)")
{
    auto const f2 = [](Result<int, Error1> r)-> Result<int, Error1>{
        return r;
    };
    using Callee = decltype(f2);
    constexpr auto non_matching_result_type = std::is_invocable_v<Callee, Result<int, Error>>;
    constexpr auto matching_result_type     = std::is_invocable_v<Callee, Result<int, Error1>>;
    REQUIRE(non_matching_result_type == false);
    REQUIRE(matching_result_type == true);
}
