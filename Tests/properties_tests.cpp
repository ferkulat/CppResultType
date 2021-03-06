//
// Created by marcel on 2/27/20.
//

#include "result_type.h"
#include "catch.hpp"

struct MayThrowWhenMoveConstructed{
    MayThrowWhenMoveConstructed(MayThrowWhenMoveConstructed&&) noexcept(false){};
    MayThrowWhenMoveConstructed& operator=(MayThrowWhenMoveConstructed const&) noexcept(false){return *this;}
    MayThrowWhenMoveConstructed()=default;
};
struct DoesNotThrowWhenMoveConstructed{
    DoesNotThrowWhenMoveConstructed(DoesNotThrowWhenMoveConstructed&&)  =default;
};

TEST_CASE("Given a Successtype that may throw when move constructed") {

    CHECK_FALSE(std::is_nothrow_move_constructible_v<MayThrowWhenMoveConstructed>);
    CHECK      (std::is_nothrow_move_constructible_v<DoesNotThrowWhenMoveConstructed>);
    CHECK      (std::is_move_constructible_v<MayThrowWhenMoveConstructed>);

}

TEST_CASE("Given a type that may throw when move constructed, when part of ResultType, then ResultType move ctor becomes nothrow(false)") {

    using ResultTypeSuccessThrows = ResultType::Result<MayThrowWhenMoveConstructed, int>;
    using ResultTypeErrorThrows = ResultType::Result<int, MayThrowWhenMoveConstructed>;

    CHECK_FALSE(std::is_nothrow_move_constructible_v<ResultTypeSuccessThrows>);
    CHECK_FALSE(std::is_nothrow_move_constructible_v<ResultTypeErrorThrows>);

}

TEST_CASE("Given a success type and error type, when both are nothrow_move_constructible, then ResultType move ctor becomes nothrow(true)") {

    // int does not throw
    using ResultTypeSuccessDoesNotThrow = ResultType::Result<DoesNotThrowWhenMoveConstructed, int>;
    using ResultTypeErrorDoesNotThrow = ResultType::Result<int, DoesNotThrowWhenMoveConstructed>;

    CHECK(std::is_nothrow_move_constructible_v<ResultTypeSuccessDoesNotThrow>);
    CHECK(std::is_nothrow_move_constructible_v<ResultTypeErrorDoesNotThrow>);

}

TEST_CASE("Make the move constructor noexcept, depending on its success and error types") {

    // int does not throw
    using ResultTypeSuccessMayThrow = ResultType::Result<MayThrowWhenMoveConstructed,int>;

    auto mayreturnsomething = [](){
        auto result = ResultTypeSuccessMayThrow{MayThrowWhenMoveConstructed{}};
        return result;
    };
    auto actual = mayreturnsomething(); //just compile without error
    CHECK(ResultType::IsSuccess(actual));

}

TEST_CASE("Make the move assignment operator noexcept, depending on its success and error types") {

    // int does not throw
    using ResultTypeSuccessMayThrow = ResultType::Result<MayThrowWhenMoveConstructed,int>;

    auto result = ResultTypeSuccessMayThrow{MayThrowWhenMoveConstructed{}};

    auto actual = ResultTypeSuccessMayThrow{1};
    actual = std::move(result); //just compile without error

}
