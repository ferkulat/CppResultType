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
            using FunctionReturnType = Result<double, ErrorType>;
            THEN("the return type is of type Result<Optional<S>, E>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(T)->Result<Optional<S>, E>"){
            using FunctionReturnType = Result<Optional<double>, ErrorType>;
            THEN("the return type is of type Result<Optional<S>, E>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }

    GIVEN("a Result<T>"){
        using PipedType = Result<int, ErrorType>;
        WHEN("piping into a function F(T)->void"){
            using FunctionReturnType = void;
            THEN("the return type is of type Result<NothingType, E>"){
                using expected = Result<NothingType, ErrorType>;
                using actual   = ReturnType_t<PipedType, FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
    GIVEN("a Result<T>"){
        using PipedType = Result<int, ErrorType>;
        WHEN("piping into a function F(T)->U"){
         using FunctionReturnType = double;
            THEN("the return type is of type Result<NothingType, E>"){
                using expected = Result<double, ErrorType>;
                using actual   = ReturnType_t<PipedType, FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
    GIVEN("a Result<T>"){
        using PipedType = Result<int, ErrorType>;
        WHEN("piping into a function F(T)->Result<U, E>"){
            using FunctionReturnType = Result<double, ErrorType>;
            THEN("the return type is of type Result<double, E>"){
                using expected = FunctionReturnType;
                using actual   = ReturnType_t<PipedType, FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
}