//
// Created by marcel on 7/21/19.
//
#include <doctest/doctest.h>
#include "result_type/pipeoperator.hpp"
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename V, typename... Os>
constexpr auto MatchType(V&& variant, Os... overloads)
{
    return std::visit(overloaded{overloads...}, std::forward<V>(variant));
}


using result_type::operator|;
using result_type::Result;
using result_type::IsSuccess;
using result_type::IsError;

enum class Domain1 {Err1, Err2};
enum class Domain2 {Err1, Err2};
struct Type2{constexpr explicit Type2(int val_):val(val_){} int val;};
struct Type3{constexpr explicit Type3(int val_):val(val_){} int val;};
struct Type4{constexpr explicit Type4(int val_):val(val_){} int val;};
struct Type1{constexpr explicit Type1(int val_):val(val_){} int val;};

using Error = std::variant<Domain1, Domain2>;

using Value = std::variant<Type1, Type2, Type3>;

auto FnReturningT2(Type1 t1)->Result<Value , Error >{
    if (t1.val) return Value{Type2{2}};
    else return Error{Domain1::Err1 };
}

auto FnTakingValue(Value value){
    using R = Result<Type4, Error >;
    return MatchType(value,
         [](Type1      )->R{return Type4{4};}
        ,[](auto       )->R{return Error{Domain2::Err2 };}
    );
}

TEST_CASE("(int | F(int)->Type1")
{
auto actual = Type1{2} |FnReturningT2|FnTakingValue;
REQUIRE( IsError(actual) );
}
