//
// Created by marcel on 6/10/20.
//

#include <type_traits>
#include <sstream>
#include "catch.hpp"
#include "result_type.h"

using ResultType::operator|;
using ResultType::Result;
using ResultType::IsSuccess;
using ResultType::IsError;

template<typename T, typename = void>
struct has_field_type : public std::false_type{};

template<typename T>
struct has_field_type<T, std::void_t<typename T::type>> : public std::true_type{};




enum class Error{Fail1, NotOdd};

TEST_CASE("enable_if its not Result and not an optional"){
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<std::optional<int>, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<std::optional<int>&, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<std::optional<int>&&, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<std::optional<int>const&, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<std::optional<int>const&&, double>>::value);

    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<Result<int, Error>, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<Result<int, Error>&, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<Result<int, Error>&&, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<Result<int, Error>const&, double>>::value);
    CHECK(false == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<Result<int, Error>const&&, double>>::value);

    CHECK(true == has_field_type<ResultType::helper::enable_if_is_not_result_and_not_option<int, double>>::value);

}

TEST_CASE("extract the value type of Result and optional") {

    CHECK(std::is_same_v<ResultType::helper::value_type_of_t<double>                              , double>);
    CHECK(std::is_same_v<ResultType::helper::value_type_of_t<std::optional<double>>               , double>);
    CHECK(std::is_same_v<ResultType::helper::value_type_of_t<Result<double, Error>>               , double>);
    CHECK(std::is_same_v<ResultType::helper::value_type_of_t<Result<std::optional<double>, Error>>, double>);

}