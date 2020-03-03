//
// Created by marcel on 3/30/17.
//
#include <type_traits>
#include "catch.hpp"
#include "result_type.h"

using ResultType::operator|;
using ResultType::Result;
using ResultType::IsSuccess;
using ResultType::IsError;

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
    bool operator==(returns const& other)const{
        return true;
    }
};

template<typename T>
struct toFunction{
    explicit toFunction(T t):value(t){}
    using type = T;
    T value;
};

template<typename T>
struct piping{
    explicit piping(T t):value(std::move(t)){}
    using type = T;
    T value;
};

template<typename ReturnType, typename PipedType, typename FunctionType >
constexpr auto IsTrue(PipedType&& pipedType, FunctionType functionType, ReturnType&& returnType){
    if constexpr (std::is_same_v<void, typename ReturnType::type>){
        return std::is_same_v<decltype(std::forward<PipedType>(pipedType).value | functionType.value),typename ReturnType::type>;
    }
    else {
        auto result = std::forward<PipedType>(pipedType).value | functionType.value;
        auto const typematch = std::is_same_v<decltype(result), typename ReturnType::type>;
        auto const value_match = result == std::forward<ReturnType>(returnType).value;
        return   value_match && typematch;

    }
}
using ResultType::NothingType;
template<class T> using ResOpt = Result<std::optional<T>, Error>;
template<class T> using Opt = std::optional<T>;
template<class T> using Res = Result<T, Error>;

TEST_CASE("Piping when function is callable with piped value returns the same type like the function")
{
    REQUIRE(IsTrue( piping{int{0}}                   , toFunction{[](int            )->void         {                            }                          }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](int val        )->float        {return            val+1.0f; }                          }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](int val        )->Opt<float>   {return            val+1.0f; }                          }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](int val        )->Res<float>   {return            val+1.0f; }                          }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](int val        )->ResOpt<float>{return Opt<float>{val+1.0f};}                          }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{int{0}}                   , toFunction{[](Opt<int>       )->void         {                                    }                  }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Opt<int> val   )->float        {return            val.value()+1.0f ;}                  }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Opt<int> val   )->Opt<float>   {return            val.value()+1.0f ;}                  }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Opt<int> val   )->Res<float>   {return            val.value()+1.0f ;}                  }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Opt<int> val   )->ResOpt<float>{return Opt<float>{val.value()+1.0f};}                  }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{Opt<int>{0}}              , toFunction{[](Opt<int>       )->void         {                                    }                  }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](Opt<int> val   )->float        {return            val.value()+1.0f ;}                  }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](Opt<int> val   )->Opt<float>   {return            val.value()+1.0f ;}                  }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](Opt<int> val   )->Res<float>   {return            val.value()+1.0f ;}                  }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](Opt<int> val   )->ResOpt<float>{return Opt<float>{val.value()+1.0f};}                  }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](Opt<int>       )->void         {                                          }            }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](Opt<int> val   )->float        {return            float(val.value_or(1)) ;}            }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](Opt<int> val   )->Opt<float>   {return            float(val.value_or(1)) ;}            }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](Opt<int> val   )->Res<float>   {return            float(val.value_or(1)) ;}            }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](Opt<int> val   )->ResOpt<float>{return Opt<float>{float(val.value_or(1))};}            }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping{int{0}}                   , toFunction{[](Res<int>       )->void         {                                          }            }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Res<int> val   )->float        {return            val.CRefSuccess()+1.0f ;}            }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Res<int> val   )->Res<float>   {return            val.CRefSuccess()+1.0f ;}            }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Res<int> val   )->Opt<float>   {return            val.CRefSuccess()+1.0f ;}            }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{int{2}}                   , toFunction{[](Res<int> val   )->ResOpt<float>{return Opt<float>{val.CRefSuccess()+1.0f};}            }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](Res<int>       )->void         {                        }                              }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](Res<int> val   )->float        {return            1.0f ;}                              }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](Res<int> val   )->Opt<float>   {return            1.0f ;}                              }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](Res<int> val   )->Res<float>   {return            1.0f ;}                              }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](Res<int> val   )->ResOpt<float>{return Opt<float>{1.0f};}                              }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping{Res<int>{0}}              , toFunction{[](Res<int>       )->void         {                                          }            }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Res<int>{2}}              , toFunction{[](Res<int> val   )->float        {return            val.CRefSuccess()+1.0f ;}            }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{Res<int>{2}}              , toFunction{[](Res<int> val   )->Res<float>   {return            val.CRefSuccess()+1.0f ;}            }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{Res<int>{2}}              , toFunction{[](Res<int> val   )->Opt<float>   {return            val.CRefSuccess()+1.0f ;}            }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{Res<int>{2}}              , toFunction{[](Res<int> val   )->ResOpt<float>{return Opt<float>{val.CRefSuccess()+1.0f};}            }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{Res<int>{Error::Fail1}}   , toFunction{[](Res<int>       )->void         {                        }                              }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Res<int>{Error::Fail1}}   , toFunction{[](Res<int> val   )->float        {return            1.0f ;}                              }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{Res<int>{Error::Fail1}}   , toFunction{[](Res<int> val   )->Opt<float>   {return            1.0f ;}                              }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Res<int>{Error::Fail1}}   , toFunction{[](Res<int> val   )->Res<float>   {return            1.0f ;}                              }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Res<int>{Error::Fail1}}   , toFunction{[](Res<int> val   )->ResOpt<float>{return Opt<float>{1.0f};}                              }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping{Opt<int>{0}}              , toFunction{[](ResOpt<int>    )->void         {                                                  }    }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](ResOpt<int> val)->float        {return            val.CRefSuccess().value()+1.0f ;}    }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](ResOpt<int> val)->Res<float>   {return            val.CRefSuccess().value()+1.0f ;}    }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](ResOpt<int> val)->Opt<float>   {return            val.CRefSuccess().value()+1.0f ;}    }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}              , toFunction{[](ResOpt<int> val)->ResOpt<float>{return Opt<float>{val.CRefSuccess().value()+1.0f};}    }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](ResOpt<int>    )->void         {                                                    }  }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](ResOpt<int> val)->float        {return        float(val.CRefSuccess().value_or(1)) ;}  }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](ResOpt<int> val)->Res<float>   {return        float(val.CRefSuccess().value_or(1)) ;}  }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](ResOpt<int> val)->Opt<float>   {return            val.CRefSuccess().value_or(1.0f) ;}  }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}              , toFunction{[](ResOpt<int> val)->ResOpt<float>{return Opt<float>{val.CRefSuccess().value_or(1.0f)};}  }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](ResOpt<int>    )->void         {                        }                              }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](ResOpt<int> val)->float        {return            1.0f ;}                              }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](ResOpt<int> val)->Opt<float>   {return            1.0f ;}                              }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](ResOpt<int> val)->Res<float>   {return            1.0f ;}                              }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{Error::Fail1}             , toFunction{[](ResOpt<int> val)->ResOpt<float>{return Opt<float>{1.0f};}                              }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping{ResOpt<int>{0}}           , toFunction{[](ResOpt<int>    )->void         {                                                  }    }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{2}}           , toFunction{[](ResOpt<int> val)->float        {return            val.CRefSuccess().value()+1.0f ;}    }, returns< float        >{           3.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{2}}           , toFunction{[](ResOpt<int> val)->Res<float>   {return            val.CRefSuccess().value()+1.0f ;}    }, returns< Res<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{2}}           , toFunction{[](ResOpt<int> val)->Opt<float>   {return            val.CRefSuccess().value()+1.0f ;}    }, returns< Opt<float>   >{           3.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{2}}           , toFunction{[](ResOpt<int> val)->ResOpt<float>{return Opt<float>{val.CRefSuccess().value()+1.0f};}    }, returns< ResOpt<float>>{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{ResOpt<int>{Opt<int>{ }}} , toFunction{[](ResOpt<int>    )->void         {                                                        } }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Opt<int>{ }}} , toFunction{[](ResOpt<int> val)->float        {return        float(val.CRefSuccess().value_or(+1))    ;} }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Opt<int>{ }}} , toFunction{[](ResOpt<int> val)->Res<float>   {return        float(val.CRefSuccess().value_or(+1))    ;} }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Opt<int>{ }}} , toFunction{[](ResOpt<int> val)->Opt<float>   {return            float(val.CRefSuccess().value_or(1)) ;} }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Opt<int>{ }}} , toFunction{[](ResOpt<int> val)->ResOpt<float>{return Opt<float>{float(val.CRefSuccess().value_or(1))};} }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue( piping{ResOpt<int>{Error::Fail1}}, toFunction{[](ResOpt<int>    )->void         {                        }                              }, returns< void         >{                } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Error::Fail1}}, toFunction{[](ResOpt<int> val)->float        {return            1.0f ;}                              }, returns< float        >{           1.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Error::Fail1}}, toFunction{[](ResOpt<int> val)->Opt<float>   {return            1.0f ;}                              }, returns< Opt<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Error::Fail1}}, toFunction{[](ResOpt<int> val)->Res<float>   {return            1.0f ;}                              }, returns< Res<float>   >{           1.0f } ));
    REQUIRE(IsTrue( piping{ResOpt<int>{Error::Fail1}}, toFunction{[](ResOpt<int> val)->ResOpt<float>{return Opt<float>{1.0f};}                              }, returns< ResOpt<float>>{Opt<float>{1.0f}} ));


    //REQUIRE(IsTrue(piping{int{2}}     , toFunction{[](ResOpt<int> val)->float{return val.CRefSuccess().value()+1.0f;}       }, returns<float>{3.0f}));
  //REQUIRE(IsTrue(piping{Opt<int>{2}}, toFunction{[](ResOpt<int> val)->float{return val.CRefSuccess().value()+1.0f;}       }, returns<float>{3.0f}));

}

TEST_CASE("Piping std::optional<T> into a function which expects T")
{
    SECTION(" and returns 'void', returns std::optional<ResultType::NothingType>")
    {
        REQUIRE(IsTrue( piping{Opt<int>{1}}, toFunction{[](int) -> void {}}, returns<Opt<NothingType> >{NothingType{}}));
        REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](int) -> void {}}, returns<Opt<NothingType> >{}));
    }

    SECTION(", wraps the function return type into std::optional")
    {
        REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](int val)->float        {return val+1.0f;}             }, returns< Opt<float>    >{           3.0f } ));
        REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< Opt<float>    >{           3.0f } ));
        REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< ResOpt<float> >{Opt<float>{3.0f}} ));
        REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float> >{Opt<float>{3.0f}} ));

        REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](int val)->float        {return val+1.0f;}             }, returns< Opt<float>    >{}                 ));
        REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< Opt<float>    >{}                 ));
        REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< ResOpt<float> >{Opt<float>{}}     ));
        REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float> >{Opt<float>{}}     ));
    }
}

TEST_CASE("Piping std::optional<T> into a function which expects Result<T, E>,  wraps the function return type into std::optional")
{
    REQUIRE(IsTrue( piping{Opt<int>{1}}, toFunction{[](Res<int>    )->void         {                              }             }, returns< Opt<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](Res<int> val)->float        {return val.CRefSuccess()+1.0f;}             }, returns< Opt<float>       >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](Res<int> val)->Opt<float>   {return val.CRefSuccess()+1.0f;}             }, returns< Opt<float>       >{           3.0f } ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](Res<int> val)->Res<float>   {return val.CRefSuccess()+1.0f;}             }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue( piping{Opt<int>{2}}, toFunction{[](Res<int> val)->ResOpt<float>{return Opt<float>{val.CRefSuccess()+1.0f};} }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](Res<int>    )->void         {                              }             }, returns< Opt<NothingType> >{}                 ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](Res<int> val)->float        {return val.CRefSuccess()+1.0f;}             }, returns< Opt<float>       >{}                 ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](Res<int> val)->Opt<float>   {return val.CRefSuccess()+1.0f;}             }, returns< Opt<float>       >{}                 ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](Res<int> val)->Res<float>   {return val.CRefSuccess()+1.0f;}             }, returns< ResOpt<float>    >{Opt<float>{}}     ));
    REQUIRE(IsTrue( piping{Opt<int>{ }}, toFunction{[](Res<int> val)->ResOpt<float>{return Opt<float>{val.CRefSuccess()+1.0f};} }, returns< ResOpt<float>    >{Opt<float>{}}     ));
}

TEST_CASE("Piping Result<T, E> into a function which expects T,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping{Res<int>{1}}            , toFunction{[](int    )->void {}                                     }, returns< Res<NothingType> >{NothingType{}}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](int    )->void {}                                     }, returns< Res<NothingType> >{Error::NotOdd}    ));

    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](int val)->float        {return val+1.0f;}             }, returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](int val)->float        {return val+1.0f;}             }, returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float>    >{Error::NotOdd}    ));
}

TEST_CASE("Piping Result<T, E> into a function which expects std::optional<T>,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping{Res<int>{1}}            , toFunction{[](Opt<int>    )->void         {}                                     }, returns< Res<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](Opt<int> val)->float        {return val.value()+1.0f;}             }, returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](Opt<int> val)->Opt<float>   {return val.value()+1.0f;}             }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](Opt<int> val)->Res<float>   {return val.value()+1.0f;}             }, returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping{Res<int>{2}}            , toFunction{[](Opt<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};} }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](Opt<int>    )->void         {}                                     }, returns< Res<NothingType> >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->float        {return val.value()+1.0f;}             }, returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->Opt<float>   {return val.value()+1.0f;}             }, returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->Res<float>   {return val.value()+1.0f;}             }, returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{Res<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};} }, returns< ResOpt<float>    >{Error::NotOdd}    ));
}


TEST_CASE("Piping Result<std::optional<T>, E> into a function which expects T,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping{ResOpt<int>{1}}            , toFunction{[](int    )->void         {}                             }, returns< ResOpt<NothingType> >{Opt<NothingType>{NothingType{}}} ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](int val)->float        {return val+1.0f;}             }, returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float>       >{Opt<float>{3.0f}}                ));

    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](int    )->void         {}                             }, returns< ResOpt<NothingType> >{Opt<NothingType>{}}              ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](int val)->float        {return val+1.0f;}             }, returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< ResOpt<float>       >{Opt<float>{}}                    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float>       >{Opt<float>{}}                    ));

    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](int    )->void         {}                             }, returns< ResOpt<NothingType> >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](int val)->float        {return val+1.0f;}             }, returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](int val)->Opt<float>   {return val+1.0f;}             }, returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](int val)->Res<float>   {return val+1.0f;}             }, returns< ResOpt<float>       >{Error::NotOdd}                   ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](int val)->ResOpt<float>{return Opt<float>{val+1.0f};} }, returns< ResOpt<float>       >{Error::NotOdd}                   ));
}

TEST_CASE("Piping Result<std::optional<T>, E> into a function which expects std::optional<T>,  wraps the function return type into Result")
{
    REQUIRE(IsTrue(piping{ResOpt<int>{1}}            , toFunction{[](Opt<int>    )->void         {}                                       }, returns< Res<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Opt<int> val)->float        {return val.value()+1.0f;}               }, returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Opt<int> val)->Opt<float>   {return val.value()+1.0f;}               }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Opt<int> val)->Res<float>   {return val.value()+1.0f;}               }, returns< Res<float>       >{           3.0f } ));
    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Opt<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};}   }, returns< ResOpt<float>    >{Opt<float>{3.0f}} ));

    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](Opt<int>    )->void         {}                                       }, returns< Res<NothingType> >{NothingType{}   } ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](Opt<int> val)->float        {return float(val.value_or(1));}         }, returns< Res<float>       >{           1.0f } ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](Opt<int> val)->Opt<float>   {return float(val.value_or(1));}         }, returns< ResOpt<float>    >{Opt<float>{1.0f}} ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](Opt<int> val)->Res<float>   {return float(val.value_or(1));}         }, returns< Res<float>       >{           1.0f } ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Opt<int>{}}}   , toFunction{[](Opt<int> val)->ResOpt<float>{return Opt<float>{val.value_or(1.0f)};} }, returns< ResOpt<float>    >{Opt<float>{1.0f}} ));

    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Opt<int>    )->void         {}                                       }, returns< Res<NothingType> >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->float        {return val.value()+1.0f;}               }, returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->Opt<float>   {return val.value()+1.0f;}               }, returns< ResOpt<float>    >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->Res<float>   {return val.value()+1.0f;}               }, returns< Res<float>       >{Error::NotOdd}    ));
    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Opt<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};}   }, returns< ResOpt<float>    >{Error::NotOdd}    ));
}

TEST_CASE("Piping Result<std::optional<T>, E> into a function which expects Result<T, E>,  wraps the function return type into Result")
{
    SECTION("This is not implemented yet")
    {
//    REQUIRE(IsTrue(piping{ResOpt<int>{1}}            , toFunction{[](Res<int>    )->void {}                                             }, returns<  Opt<NothingType> >{NothingType{}}    ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Res<int> val)->float{return val.CRefSuccess()+1.0f;}               }, returns<  Opt<float>       >{3.0f}             ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Res<int> val)->Opt<float>{return val.value()+1.0f;}                }, returns<  Opt<float>       >{Opt<float>{3.0f}} ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Res<int> val)->Res<float>{return val.value()+1.0f;}                }, returns<  ResOpt<float>    >{3.0f}             ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{2}}            , toFunction{[](Res<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};} }, returns<  ResOpt<float>    >{Opt<float>{3.0f}} ));

//    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Res<int>    )->void {}                                             }, returns<  Opt<NothingType> >{}                 ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Res<int> val)->float{return 1.0f;}                                 }, returns<  Opt<float>       >{1.0f}             ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Res<int> val)->Opt<float>{return val.value()+1.0f;}                }, returns<  ResOpt<float>    >{Error::NotOdd}    ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Res<int> val)->Res<float>{return val.value()+1.0f;}                }, returns<  Res<float>       >{Error::NotOdd}    ));
//    REQUIRE(IsTrue(piping{ResOpt<int>{Error::NotOdd}}, toFunction{[](Res<int> val)->ResOpt<float>{return Opt<float>{val.value()+1.0f};} }, returns<  ResOpt<float>    >{Error::NotOdd}    ));
    }
}

