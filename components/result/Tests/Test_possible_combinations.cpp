//
// Created by marcel on 3/30/17.
//
#include <type_traits>
#include <catch2/catch.hpp>
#include "result_type/pipeoperator.hpp"

using result_type::operator|;
using result_type::Result;
using result_type::IsSuccess;
using result_type::IsError;

enum class Error{Fail1, NotOdd};

template<typename T>
struct returns{
    returns()=default;
    explicit returns(T t):value(std::move(t)){}
    //returns(T&& t):value(std::move(t)){}
    using type = T;
    T value;
    bool operator==(returns const& other)const{
        return value == other.value;
    }
};
template<>
struct  returns<void>{
    using type = void;
    bool operator==(returns const& /*other*/)const{
        return true;
    }
};

template<typename T>
struct FunctionType{
    explicit FunctionType(T t):value(t){}
    using type = T;
    T value;
};
template<typename T>
auto toFunction(T&&t){
  return FunctionType<T>(std::forward<T>(t));
}
template<typename T>
struct PipedType{
    explicit PipedType(T t):value(std::move(t)){}
    using type = T;
    T value;
};
template<typename T>
auto piping(T&&t){
  return  PipedType<T>(std::forward<T>(t));
}

template<typename ReturnType, typename PipedType, typename FunctionType, class ENABLER = void  >
struct IsTrueImpl;

template<typename ReturnType, typename PipedType, typename FunctionType>
struct IsTrueImpl<ReturnType, PipedType, FunctionType, std::enable_if_t<!std::is_same<void, typename ReturnType::type>::value, void>>{
  static bool get(PipedType&& pipedType, FunctionType functionType, [[maybe_unused]] ReturnType&& returnType){
        auto result = std::forward<PipedType>(pipedType).value | functionType.value;
        auto const typematch = std::is_same<decltype(result), typename ReturnType::type>::value;
        auto const value_match = result == std::forward<ReturnType>(returnType).value;
        return   value_match && typematch;

  }
};
template<typename ReturnType, typename PipedType, typename FunctionType>
struct IsTrueImpl<ReturnType, PipedType, FunctionType, std::enable_if_t<std::is_same<void, typename ReturnType::type>::value, void>>{
  static bool get(PipedType&& pipedType, FunctionType functionType, [[maybe_unused]] ReturnType&& returnType){
        return std::is_same<decltype(std::forward<PipedType>(pipedType).value | functionType.value),typename ReturnType::type>::value;
  }
};


template<typename ReturnType, typename PipedType, typename FunctionType >
constexpr auto IsTrue(PipedType&& pipedType, FunctionType functionType, [[maybe_unused]] ReturnType&& returnType){
  return IsTrueImpl<ReturnType, PipedType, FunctionType>::get(std::forward<PipedType>(pipedType), std::forward<FunctionType>(functionType), std::forward<ReturnType>(returnType));
}
using result_type::NothingType;
template<class T> using ResOpt = Result<result_type::Optional<T>, Error>;
template<class T> using Opt = result_type::Optional<T>;
template<class T> using Res = Result<T, Error>;

TEST_CASE("Piping when function is callable with piped value returns the same type like the function")
{
    REQUIRE(IsTrue( piping(int{0})                   , toFunction([](int            )->void         {                                   }         ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](int val        )->float        {return            float(val)+1.0f; }         ), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](int val        )->Opt<float>   {return            float(val)+1.0f; }         ), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](int val        )->Res<float>   {return            float(val)+1.0f; }         ), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](int val        )->ResOpt<float>{return Opt<float>{float(val)+1.0f};}         ), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(int{0})                   , toFunction([](Opt<int>       )->void         {                                           } ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Opt<int> val   )->float        {return            float(val.value())+1.0f ;} ), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Opt<int> val   )->Opt<float>   {return            float(val.value())+1.0f ;} ), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Opt<int> val   )->Res<float>   {return            float(val.value())+1.0f ;} ), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Opt<int> val   )->ResOpt<float>{return Opt<float>{float(val.value())+1.0f};} ), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(Opt<int>{0})              , toFunction([](Opt<int>       )->void         {                                           } ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](Opt<int> val   )->float        {return            float(val.value())+1.0f ;} ), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](Opt<int> val   )->Opt<float>   {return            float(val.value())+1.0f ;} ), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](Opt<int> val   )->Res<float>   {return            float(val.value())+1.0f ;} ), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](Opt<int> val   )->ResOpt<float>{return Opt<float>{float(val.value())+1.0f};} ), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](Opt<int>       )->void         {                                          }  ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](Opt<int> val   )->float        {return            float(val.value_or(1)) ;}  ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](Opt<int> val   )->Opt<float>   {return            float(val.value_or(1)) ;}  ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](Opt<int> val   )->Res<float>   {return            float(val.value_or(1)) ;}  ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](Opt<int> val   )->ResOpt<float>{return Opt<float>{float(val.value_or(1))};}  ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping(int{0})                   , toFunction([](Res<int>       )->void         {                                                 } ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Res<int> val   )->float        {return            float(val.CRefSuccess())+1.0f ;} ), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Res<int> val   )->Res<float>   {return            float(val.CRefSuccess())+1.0f ;} ), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Res<int> val   )->Opt<float>   {return            float(val.CRefSuccess())+1.0f ;} ), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(int{2})                   , toFunction([](Res<int> val   )->ResOpt<float>{return Opt<float>{float(val.CRefSuccess())+1.0f};} ), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](Res<int>       )->void         {                        }                          ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](Res<int>       )->float        {return            1.0f ;}                          ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](Res<int>       )->Opt<float>   {return            1.0f ;}                          ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](Res<int>       )->Res<float>   {return            1.0f ;}                          ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](Res<int>       )->ResOpt<float>{return Opt<float>{1.0f};}                          ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping(Res<int>{0})              , toFunction([](Res<int>       )->void         {                                                 } ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Res<int>{2})              , toFunction([](Res<int> val   )->float        {return            float(val.CRefSuccess())+1.0f ;} ), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(Res<int>{2})              , toFunction([](Res<int> val   )->Res<float>   {return            float(val.CRefSuccess())+1.0f ;} ), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(Res<int>{2})              , toFunction([](Res<int> val   )->Opt<float>   {return            float(val.CRefSuccess())+1.0f ;} ), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(Res<int>{2})              , toFunction([](Res<int> val   )->ResOpt<float>{return Opt<float>{float(val.CRefSuccess())+1.0f};} ), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(Res<int>{Error::Fail1})   , toFunction([](Res<int>       )->void         {                        }                          ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Res<int>{Error::Fail1})   , toFunction([](Res<int>       )->float        {return            1.0f ;}                          ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(Res<int>{Error::Fail1})   , toFunction([](Res<int>       )->Opt<float>   {return            1.0f ;}                          ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Res<int>{Error::Fail1})   , toFunction([](Res<int>       )->Res<float>   {return            1.0f ;}                          ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Res<int>{Error::Fail1})   , toFunction([](Res<int>       )->ResOpt<float>{return Opt<float>{1.0f};}                          ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping(Opt<int>{0})              , toFunction([](ResOpt<int>    )->void         {                                                         }), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](ResOpt<int> val)->float        {return            float(val.CRefSuccess().value())+1.0f ;}), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](ResOpt<int> val)->Res<float>   {return            float(val.CRefSuccess().value())+1.0f ;}), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](ResOpt<int> val)->Opt<float>   {return            float(val.CRefSuccess().value())+1.0f ;}), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2})              , toFunction([](ResOpt<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess().value())+1.0f};}), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](ResOpt<int>    )->void         {                                                        } ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](ResOpt<int> val)->float        {return            float(val.CRefSuccess().value_or(1)) ;} ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](ResOpt<int> val)->Res<float>   {return            float(val.CRefSuccess().value_or(1)) ;} ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](ResOpt<int> val)->Opt<float>   {return            float(val.CRefSuccess().value_or(1)) ;} ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{ })              , toFunction([](ResOpt<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess().value_or(1))};} ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](ResOpt<int>    )->void         {                        }                                 ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](ResOpt<int>    )->float        {return            1.0f ;}                                 ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](ResOpt<int>    )->Opt<float>   {return            1.0f ;}                                 ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](ResOpt<int>    )->Res<float>   {return            1.0f ;}                                 ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(Error::Fail1)             , toFunction([](ResOpt<int>    )->ResOpt<float>{return Opt<float>{1.0f};}                                 ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping(ResOpt<int>{0})           , toFunction([](ResOpt<int>    )->void         {                                                         }), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{2})           , toFunction([](ResOpt<int> val)->float        {return            float(val.CRefSuccess().value())+1.0f ;}), returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{2})           , toFunction([](ResOpt<int> val)->Res<float>   {return            float(val.CRefSuccess().value())+1.0f ;}), returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{2})           , toFunction([](ResOpt<int> val)->Opt<float>   {return            float(val.CRefSuccess().value())+1.0f ;}), returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{2})           , toFunction([](ResOpt<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess().value())+1.0f};}), returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping(ResOpt<int>{Opt<int>{ }}) , toFunction([](ResOpt<int>    )->void         {                                                        } ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Opt<int>{ }}) , toFunction([](ResOpt<int> val)->float        {return            float(val.CRefSuccess().value_or(+1));} ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Opt<int>{ }}) , toFunction([](ResOpt<int> val)->Res<float>   {return            float(val.CRefSuccess().value_or(+1));} ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Opt<int>{ }}) , toFunction([](ResOpt<int> val)->Opt<float>   {return            float(val.CRefSuccess().value_or(1)) ;} ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Opt<int>{ }}) , toFunction([](ResOpt<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess().value_or(1))};} ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping(ResOpt<int>{Error::Fail1}), toFunction([](ResOpt<int>    )->void         {                        }                                 ), returns< void         >{                } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Error::Fail1}), toFunction([](ResOpt<int>    )->float        {return            1.0f ;}                                 ), returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Error::Fail1}), toFunction([](ResOpt<int>    )->Opt<float>   {return            1.0f ;}                                 ), returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Error::Fail1}), toFunction([](ResOpt<int>    )->Res<float>   {return            1.0f ;}                                 ), returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping(ResOpt<int>{Error::Fail1}), toFunction([](ResOpt<int>    )->ResOpt<float>{return Opt<float>{1.0f};}                                 ), returns< ResOpt<float>>{Opt<float>{1.0f}} ));


    //REQUIRE(IsTrue(piping(int{2}}     , toFunction{[](ResOpt<int> val)->float{return val.CRefSuccess().value()+1.0f;}       }, returns<float>{3.0f}));
  //REQUIRE(IsTrue(piping(Opt<int>{2}}, toFunction{[](ResOpt<int> val)->float{return val.CRefSuccess().value()+1.0f;}       }, returns<float>{3.0f}));

}
TEST_CASE("Piping std::optional<T> into a function which expects T") {
    SECTION(" and returns 'void', returns std::optional<ResultType::NothingType>") {
        REQUIRE(IsTrue(piping(Opt<int>{1}), toFunction([](int) -> void {}), returns<Opt<NothingType> >{NothingType{}}));
        REQUIRE(IsTrue(piping(Opt<int>{}), toFunction([](int) -> void {}), returns<Opt<NothingType> >{}));
    }
}  /*Marcel

    SECTION(", wraps the function return type into std::optional")
    {
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int val)->float        {return float(val)+1.0f            ;} ), returns< Opt<float>    >{           3.0f } ));
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int val)->Opt<float>   {return float(val)+1.0f            ;} ), returns< Opt<float>    >{           3.0f } ));
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int    )->Opt<float>   {return Opt<float>()               ;} ), returns< Opt<float>    >{                } ));
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int val)->Res<float>   {return float(val)+1.0f            ;} ), returns< ResOpt<float> >{Opt<float>{3.0f}} ));
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float> >{Opt<float>{3.0f}} ));
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int    )->ResOpt<float>{return Opt<float>{}               ;} ), returns< ResOpt<float> >{Opt<float>{}    } ));
        REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](int    )->ResOpt<float>{return Error::Fail1               ;} ), returns< ResOpt<float> >{Error::Fail1    } ));

        REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](int val)->float        {return float(val)+1.0f            ;} ), returns< Opt<float>    >{}                 ));
        REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](int val)->Opt<float>   {return float(val)+1.0f            ;} ), returns< Opt<float>    >{}                 ));
        REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](int val)->Res<float>   {return float(val)+1.0f            ;} ), returns< ResOpt<float> >{Opt<float>{}}     ));
        REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float> >{Opt<float>{}}     ));
    }
}

TEST_CASE("Piping std::optional<T> into a function which expects Result<T, E>,  wraps the function return type into std::optional")
{
    REQUIRE(IsTrue( piping(Opt<int>{1}), toFunction([](Res<int>    )->void         {                              }                    ), returns< Opt<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int> val)->float        {return float(val.CRefSuccess())+1.0f;}             ), returns< Opt<float>       >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int> val)->Opt<float>   {return float(val.CRefSuccess())+1.0f;}             ), returns< Opt<float>       >{           3.0f } ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int>    )->Opt<float>   {return std::nullopt          ;}                    ), returns< Opt<float>       >{                } ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int> val)->Res<float>   {return float(val.CRefSuccess())+1.0f;}             ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int>    )->Res<float>   {return Error::Fail1          ;}                    ), returns< ResOpt<float>    >{Error::Fail1    } ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess())+1.0f};} ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int>    )->ResOpt<float>{return Opt<float>{}          ;}                    ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue( piping(Opt<int>{2}), toFunction([](Res<int>    )->ResOpt<float>{return Error::Fail1          ;}                    ), returns< ResOpt<float>    >{Error::Fail1    } ));

    REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](Res<int>    )->void         {                                     }             ), returns< Opt<NothingType> >{}                 ));
    REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](Res<int> val)->float        {return float(val.CRefSuccess())+1.0f;}             ), returns< Opt<float>       >{}                 ));
    REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](Res<int> val)->Opt<float>   {return float(val.CRefSuccess())+1.0f;}             ), returns< Opt<float>       >{}                 ));
    REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](Res<int> val)->Res<float>   {return float(val.CRefSuccess())+1.0f;}             ), returns< ResOpt<float>    >{Opt<float>{}}     ));
    REQUIRE(IsTrue( piping(Opt<int>{ }), toFunction([](Res<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess())+1.0f};} ), returns< ResOpt<float>    >{Opt<float>{}}     ));
}

TEST_CASE("Piping Result<T, E> into a function which expects T,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping(Res<int>{1})            , toFunction([](int    )->void {}                                     ), returns< Res<NothingType> >{NothingType{}}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](int    )->void {}                                     ), returns< Res<NothingType> >{Error::NotOdd}    ));

    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int val)->float        {return float(val)+1.0f;}             ), returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int val)->Opt<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int    )->Opt<float>   {return std::nullopt;}                ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int val)->Res<float>   {return float(val)+1.0f;}             ), returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int    )->Res<float>   {return Error::Fail1;}                ), returns< Res<float>       >{Error::Fail1    } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int    )->ResOpt<float>{return Opt<float>{};}                ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](int    )->ResOpt<float>{return Error::Fail1;}                ), returns< ResOpt<float>    >{Error::Fail1    } ));

    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](int val)->float        {return float(val)+1.0f;}             ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](int val)->Opt<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](int val)->Res<float>   {return float(val)+1.0f;}             ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float>    >{Error::NotOdd}    ));
}

TEST_CASE("Piping Result<T, E> into a function which expects std::optional<T>,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping(Res<int>{1})            , toFunction([](Opt<int>    )->void         {}                                            ), returns< Res<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int> val)->float        {return float(val.value())+1.0f;}             ), returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int> val)->Opt<float>   {return float(val.value())+1.0f;}             ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int>    )->Opt<float>   {return std::nullopt    ;}                    ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int> val)->Res<float>   {return float(val.value())+1.0f;}             ), returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int>    )->Res<float>   {return Error::Fail1    ;}                    ), returns< Res<float>       >{Error::Fail1    } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int> val)->ResOpt<float>{return Opt<float>{float(val.value())+1.0f};} ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int>    )->ResOpt<float>{return Opt<float>{}                       ;} ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue(piping(Res<int>{2})            , toFunction([](Opt<int>    )->ResOpt<float>{return Error::Fail1                       ;} ), returns< ResOpt<float>    >{Error::Fail1    } ));

    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](Opt<int>    )->void         {}                                            ), returns< Res<NothingType> >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](Opt<int> val)->float        {return float(val.value())+1.0f;}             ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](Opt<int> val)->Opt<float>   {return float(val.value())+1.0f;}             ), returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](Opt<int> val)->Res<float>   {return float(val.value())+1.0f;}             ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(Res<int>{Error::NotOdd}), toFunction([](Opt<int> val)->ResOpt<float>{return Opt<float>{float(val.value())+1.0f};} ), returns< ResOpt<float>    >{Error::NotOdd}    ));
}


TEST_CASE("Piping Result<std::optional<T>, E> into a function which expects T,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping(ResOpt<int>{1})            , toFunction([](int    )->void         {}                                    ), returns< ResOpt<NothingType> >{Opt<NothingType>{NothingType{}}} ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int val)->float        {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int val)->Opt<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int    )->Opt<float>   {return std::nullopt;}                ), returns< ResOpt<float>       >{Opt<float>{std::nullopt}}        ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int val)->Res<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int    )->Res<float>   {return Error::NotOdd;}               ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int    )->ResOpt<float>{return Opt<float>{}        ;}        ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](int    )->ResOpt<float>{return Error::NotOdd;}               ), returns< ResOpt<float>       >{Error::NotOdd}                   ));

    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int    )->void         {}                                    ), returns< ResOpt<NothingType> >{Opt<NothingType>{}}              ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int val)->float        {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int val)->Opt<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int    )->Opt<float>   {return std::nullopt;}                ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int val)->Res<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int    )->Res<float>   {return Error::NotOdd;}               ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int    )->ResOpt<float>{return Opt<float>{}        ;}        ), returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](int    )->ResOpt<float>{return Error::NotOdd       ;}        ), returns< ResOpt<float>       >{Opt<float>{}}                    ));

    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int    )->void         {}                                    ), returns< ResOpt<NothingType> >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int val)->float        {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int val)->Opt<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int    )->Opt<float>   {return std::nullopt;}                ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int val)->Res<float>   {return float(val)+1.0f;}             ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int    )->Res<float>   {return Error::Fail1;}                ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int val)->ResOpt<float>{return Opt<float>{float(val)+1.0f};} ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int    )->ResOpt<float>{return Opt<float>{};}                ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](int    )->ResOpt<float>{return Error::Fail1;}                ), returns< ResOpt<float>       >{Error::NotOdd}                   ));
}

TEST_CASE("Piping Result<std::optional<T>, E> into a function which expects std::optional<T>,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping(ResOpt<int>{1})            , toFunction([](Opt<int>    )->void         {}                                              ), returns< Res<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int> val)->float        {return float(val.value())+1.0f;}               ), returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int> val)->Opt<float>   {return float(val.value())+1.0f;}               ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int>    )->Opt<float>   {return std::nullopt;}                          ), returns< ResOpt<float>    >{Opt<float>{}}     ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int> val)->Res<float>   {return float(val.value())+1.0f;}               ), returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int>    )->Res<float>   {return Error::Fail1;}                          ), returns< Res<float>       >{Error::Fail1    } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int> val)->ResOpt<float>{return Opt<float>{float(val.value())+1.0f};}   ), returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int>    )->ResOpt<float>{return Opt<float>{}                       ;}   ), returns< ResOpt<float>    >{Opt<float>{    }} ));
    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Opt<int>    )->ResOpt<float>{return Error::Fail1                       ;}   ), returns< ResOpt<float>    >{Error::Fail1    } ));

    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int>    )->void         {}                                           ), returns< Res<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int> val)->float        {return float(val.value_or(1));}             ), returns< Res<float>       >{           1.0f } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int> val)->Opt<float>   {return float(val.value_or(1));}             ), returns< ResOpt<float>    >{Opt<float>{1.0f}} ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int>    )->Opt<float>   {return std::nullopt          ;}             ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int> val)->Res<float>   {return float(val.value_or(1));}             ), returns< Res<float>       >{           1.0f } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int>    )->Res<float>   {return Error::Fail1          ;}             ), returns< Res<float>       >{Error::Fail1    } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int> val)->ResOpt<float>{return Opt<float>{float(val.value_or(1))};} ), returns< ResOpt<float>    >{Opt<float>{1.0f}} ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int>    )->ResOpt<float>{return Opt<float>{}                      ;} ), returns< ResOpt<float>    >{Opt<float>{}    } ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Opt<int>{}})   , toFunction([](Opt<int>    )->ResOpt<float>{return Error::Fail1                      ;} ), returns< ResOpt<float>    >{Error::Fail1    } ));

    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int>    )->void         {}                                              ), returns< Res<NothingType> >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int> val)->float        {return float(val.value())+1.0f;}               ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int> val)->Opt<float>   {return float(val.value())+1.0f;}               ), returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int>    )->Opt<float>   {return std::nullopt    ;}                      ), returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int> val)->Res<float>   {return float(val.value())+1.0f;}               ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int>    )->Res<float>   {return Error::Fail1    ;}                      ), returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int> val)->ResOpt<float>{return Opt<float>{float(val.value())+1.0f};}   ), returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int>    )->ResOpt<float>{return Opt<float>{}    ;}                      ), returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Opt<int>    )->ResOpt<float>{return Error::Fail1    ;}                      ), returns< ResOpt<float>    >{Error::NotOdd}    ));
}

TEST_CASE("Piping Result<std::optional<T>, E> into a function which expects Result<T, E>,  wraps the function return type into Result")
{
    SECTION("This is not implemented yet")
    {
//    REQUIRE(IsTrue(piping(ResOpt<int>{1})            , toFunction([](Res<int>    )->void {}                                             ), returns<  Opt<NothingType> >{NothingType{}}    ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Res<int> val)->float{return val.CRefSuccess()+1.0f;}               ), returns<  Opt<float>       >{3.0f}             ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Res<int> val)->Opt<float>{return val.value()+1.0f;}                ), returns<  Opt<float>       >{Opt<float>{3.0f}} ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Res<int> val)->Res<float>{return val.value()+1.0f;}                ), returns<  ResOpt<float>    >{3.0f}             ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{2})            , toFunction([](Res<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};} ), returns<  ResOpt<float>    >{Opt<float>{3.0f}} ));

//    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Res<int>    )->void {}                                             ), returns<  Opt<NothingType> >{}                 ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Res<int> val)->float{return 1.0f;}                                 ), returns<  Opt<float>       >{1.0f}             ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Res<int> val)->Opt<float>{return val.value()+1.0f;}                ), returns<  ResOpt<float>    >{Error::NotOdd}    ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Res<int> val)->Res<float>{return val.value()+1.0f;}                ), returns<  Res<float>       >{Error::NotOdd}    ));
//    REQUIRE(IsTrue(piping(ResOpt<int>{Error::NotOdd}), toFunction([](Res<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};} ), returns<  ResOpt<float>    >{Error::NotOdd}    ));
    }
}
    */

