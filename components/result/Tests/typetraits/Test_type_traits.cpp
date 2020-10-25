//#define CATCH_CONFIG_MAIN
#include <doctest/doctest.h>
#include <result_type/typetraits.hpp>
#include <result_type/result.hpp>
#include <result_type/detail/returntype.hpp>
using result_type::Result;
using result_type::Optional;
using result_type::NothingType;

enum class ErrorType {lol};
TEST_SUITE("Testing isResultTypeWithNonOptional"){
    TEST_CASE("passing Result<Optional<int>, ErrorType>, returns false"){
        using InputType = result_type::Result<Optional<int>,  ErrorType>;
        REQUIRE_FALSE(result_type::isResultTypeWithNonOptional<InputType>::value);
    }
    TEST_CASE("passing Result<int, ErrorType>, returns true"){
        using InputType = Result< int,  ErrorType>;
        REQUIRE(result_type::isResultTypeWithNonOptional<InputType>::value);
    }
}
TEST_SUITE("Testing isResultTypeWithOptional"){
    TEST_CASE("passing Result<Optional<int>, ErrorType>, returns false"){
        using InputType = result_type::Result<Optional<int>,  ErrorType>;
        REQUIRE(result_type::isResultTypeWithOptional<InputType>::value);
    }
    TEST_CASE("passing Result<int, ErrorType>, returns true"){
        using InputType = Result< int,  ErrorType>;
        REQUIRE_FALSE(result_type::isResultTypeWithOptional<InputType>::value);
    }
}

SCENARIO("Testing ReturnType_t"){
    using result_type::detail::ReturnType_t;
    GIVEN("an Optional<T>"){
        using PipedType = Optional<int>;
        WHEN("piping into a function F(T)->Result<S, E>"){
            auto function = [](int )->Result<double, ErrorType>{return 3.0;};

            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<Optional<S>, E>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(T)->Result<Optional<S>, E>"){
            auto function = [](int )->Result<Optional<double>, ErrorType>{return Optional<double>(3.0);};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<Optional<S>, E>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }

    GIVEN("a Result<T>"){
        using PipedType = Result<int, ErrorType>;
        WHEN("piping into a function F(T)->void"){
            auto function = [](int )->void{};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<NothingType, E>"){
                using expected = Result<NothingType, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
    GIVEN("a Result<T>"){
        using PipedType = Result<int, ErrorType>;
        WHEN("piping into a function F(T)->U"){
            auto function = [](int )->double{return 3.0;};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<NothingType, E>"){
                using expected = Result<double, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
    GIVEN("a Result<T>"){
        using PipedType = Result<int, ErrorType>;
        WHEN("piping into a function F(T)->Result<U, E>"){
            auto function = [](int )->Result<double, ErrorType>{return 3.0;};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<double, E>"){
                using expected = FunctionReturnType;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
    GIVEN("a Result<Optional<<T>,E>"){
        using PipedType = Result<Optional<int>, ErrorType>;
        WHEN("piping into a function F(T)->void"){
            auto function = [](int )->void{};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<Optional<NothingType>, E>"){
                using expected = Result<Optional<NothingType>, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(T)->U"){
            auto function = [](int )->double{return 4.0;};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<NothingType, E>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(T)->U"){
            auto function = [](int )->double{return 4.0;};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<NothingType, E>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
}