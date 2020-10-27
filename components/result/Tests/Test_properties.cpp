//
// Created by marcel on 2/27/20.
//

#include "result_type/result.hpp"
#include <doctest/doctest.h>

struct MayThrowWhenMoveConstructed{
    MayThrowWhenMoveConstructed(MayThrowWhenMoveConstructed&&) noexcept(false){};
    MayThrowWhenMoveConstructed& operator=(MayThrowWhenMoveConstructed const&) noexcept(false){return *this;}
    MayThrowWhenMoveConstructed()=default;
};
struct DoesNotThrowWhenMoveConstructed{
    DoesNotThrowWhenMoveConstructed(DoesNotThrowWhenMoveConstructed&&)  =default;
};

TEST_CASE("Given a Successtype that may throw when move constructed") {

    CHECK_FALSE(std::is_nothrow_move_constructible<MayThrowWhenMoveConstructed>::value);
    CHECK      (std::is_nothrow_move_constructible<DoesNotThrowWhenMoveConstructed>::value);
    CHECK      (std::is_move_constructible<MayThrowWhenMoveConstructed>::value);

}

TEST_CASE("Given a type that may throw when move constructed, when part of ResultType, then ResultType move ctor becomes nothrow(false)") {

    using ResultTypeSuccessThrows = result_type::Result<MayThrowWhenMoveConstructed, int>;
    using ResultTypeErrorThrows = result_type::Result<int, MayThrowWhenMoveConstructed>;

    CHECK_FALSE(std::is_nothrow_move_constructible<ResultTypeSuccessThrows>::value);
    CHECK_FALSE(std::is_nothrow_move_constructible<ResultTypeErrorThrows>::value);

}

TEST_CASE("Given a success type and error type, when both are nothrow_move_constructible, then ResultType move ctor becomes nothrow(true)") {

    // int does not throw
    using ResultTypeSuccessDoesNotThrow = result_type::Result<DoesNotThrowWhenMoveConstructed, int>;
    using ResultTypeErrorDoesNotThrow = result_type::Result<int, DoesNotThrowWhenMoveConstructed>;

    CHECK(std::is_nothrow_move_constructible<ResultTypeSuccessDoesNotThrow>::value);
    CHECK(std::is_nothrow_move_constructible<ResultTypeErrorDoesNotThrow>::value);

}

TEST_CASE("Make the move constructor noexcept, depending on its success and error types") {

    // int does not throw
    using ResultTypeSuccessMayThrow = result_type::Result<MayThrowWhenMoveConstructed,int>;

    auto mayreturnsomething = [](){
        auto result = ResultTypeSuccessMayThrow{MayThrowWhenMoveConstructed{}};
        return result;
    };
    auto actual = mayreturnsomething(); //just compile without error
    CHECK(result_type::isSuccess(actual));

}

TEST_CASE("Make the move assignment operator noexcept, depending on its success and error types") {

    // int does not throw
    using ResultTypeSuccessMayThrow = result_type::Result<MayThrowWhenMoveConstructed,int>;

    auto result = ResultTypeSuccessMayThrow{MayThrowWhenMoveConstructed{}};

    auto actual = ResultTypeSuccessMayThrow{1};
    actual = std::move(result); //just compile without error

}
