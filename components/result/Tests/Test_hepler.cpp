//
// Created by marcel on 6/10/20.
//

#include <type_traits>
#include <sstream>
#include <doctest/doctest.h>
#include "result_type/helper.hpp"
#include <result_type/detail/value_type_of.hpp>

using result_type::Result;
using result_type::isSuccess;
using result_type::isError;
using result_type::Optional;

template<typename T, typename = void>
struct has_field_type : public std::false_type{};

template<typename T>
struct has_field_type<T, std::void_t<typename T::type>> : public std::true_type{};




enum class Error{Fail1, NotOdd};

TEST_CASE("enable_if its not Result and not an optional"){
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Optional<int>, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Optional<int>&, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Optional<int>&&, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Optional<int>const&, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Optional<int>const&&, double>>::value);

    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Result<int, Error>, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Result<int, Error>&, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Result<int, Error>&&, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Result<int, Error>const&, double>>::value);
    CHECK(false == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<Result<int, Error>const&&, double>>::value);

    CHECK(true == has_field_type<result_type::helper::enable_if_is_not_result_and_not_option<int, double>>::value);

}

TEST_CASE("extract the value type of Result and optional") {

    CHECK(std::is_same<result_type::detail::value_type_of_t<double>                         , double>::value);
    CHECK(std::is_same<result_type::detail::value_type_of_t<Optional<double>>               , double>::value);
    CHECK(std::is_same<result_type::detail::value_type_of_t<Result<double, Error>>          , double>::value);
    CHECK(std::is_same<result_type::detail::value_type_of_t<Result<Optional<double>, Error>>, double>::value);

}