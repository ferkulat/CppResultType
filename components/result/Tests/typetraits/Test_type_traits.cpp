//#define CATCH_CONFIG_MAIN
#include <doctest/doctest.h>
#include <result_type/typetraits.hpp>
#include <result_type/result.hpp>
#include <result_type/detail/returntype.hpp>
#include <string>
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
        WHEN("piping into a function F(Optional<T>)->U"){
            auto function = [](Optional<int> )->double{return 4.0;};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<double, E>"){
                using expected = Result<FunctionReturnType, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(Optional<T>)->Result<U, E>"){
            auto function = [](Optional<int> )->Result<double, ErrorType>{return 4.0;};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<double, Er>"){
                using expected = Result<double, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(Optional<T>)->void"){
            auto function = [](Optional<int> )->void{};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<NothingType, Er>"){
                using expected = Result<NothingType, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
        WHEN("piping into a function F(Optional<T>)->Result<Optional<U>, E>"){
            auto function = [](Optional<int> )->Result<Optional<double>, ErrorType>{return Optional<double>(4.0);};
            using FunctionReturnType = decltype(function(2));
            THEN("the return type is of type Result<double, Er>"){
                using expected = Result<Optional<double>, ErrorType>;
                using actual   = ReturnType_t<PipedType, decltype(function), FunctionReturnType>;
                REQUIRE(std::is_same<expected, actual>::value);
            }
        }
    }
}


TEST_SUITE("Testing is_result<T>"){
    TEST_CASE("T = Result<U,E>&, should return true"){
        auto const actual = result_type::is_result_type<result_type::Result<double, ErrorType>&>::value;
        REQUIRE(actual);
    }
}

TEST_SUITE("Testing isInvokable"){
    TEST_CASE("lol"){
        auto const callable = [](std::string& str){return str.size();};
        using ArgType = result_type::Result<std::string, ErrorType>&;

        auto const actual = result_type::isInvokeable<decltype(callable),decltype(std::declval<ArgType>().Success())>::value;
                REQUIRE(actual);
    }
}

TEST_SUITE("Testing ArgumentIsResultButFunctionAcceptsItsSuccessType"){
    TEST_CASE("With Result<double, ErrorType>& and f(double&) it should match"){
        auto  callable = [](double& d){ return d+3;};
        using ArgType = result_type::Result<double, ErrorType>&;
        using CalleeType = decltype(callable);
        auto const actual =  result_type::ArgumentIsResultWithNonOptionalButFunctionAcceptsItsSuccessType<ArgType , CalleeType>::value;

        REQUIRE(!result_type::isInvokeable<CalleeType, ArgType>::value);
        REQUIRE(result_type::isResultTypeWithNonOptional<ArgType>::value);
        REQUIRE(result_type::isInvokeable<CalleeType, decltype(std::declval<ArgType>().Success())>::value);
        REQUIRE(actual);
    }
}

TEST_SUITE("Testing isResultTypeWithNonOptional"){
    using result_type::isResultTypeWithNonOptional;
    TEST_CASE("With Result<double, ErrorType>& and f(double&) it should match"){
        using ArgType = result_type::Result<double, ErrorType>&;
        auto const actual =  result_type::isResultTypeWithNonOptional<ArgType>::value;

        REQUIRE(result_type::is_result_type<ArgType>::value);
        REQUIRE(!result_type::is_optional_type< decltype(std::declval<ArgType>().Success())>::value);
        REQUIRE(actual);
    }
}
TEST_SUITE("Testing isResultTypeWithNonOptional"){
    using result_type::isResultTypeWithOptional;
    TEST_CASE("With Result<double, ErrorType>& and f(double&) it should match"){
        using ArgType = result_type::Result<result_type::Optional<double>, ErrorType>&;
        auto const actual =  result_type::isResultTypeWithOptional<ArgType>::value;

        REQUIRE(actual);
    }
}