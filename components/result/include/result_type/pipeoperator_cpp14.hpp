//
// Created by User on 10/24/2020.
//

#ifndef CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
#define CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
#include <type_traits>
#include <result_type/result.hpp>
#include <result_type/typetraits.hpp>

namespace result_type::detail{

    template<typename ArgType, typename Callee, class ENABLER = void>
    struct call;


    template<typename Callee, typename ArgType>
    using accepts = decltype(std::declval<Callee>()(std::declval<std::remove_reference_t<ArgType>>()));

    template<typename Callee, typename ArgType, typename = void>
    struct isInvokeable : std::false_type{};

    template<typename Callee, typename ArgType>
    struct isInvokeable<Callee, ArgType, std::void_t<accepts<Callee, ArgType>>>:std::true_type {};

    template<typename Callee, typename ArgType, typename ReturnType, typename = void>
    struct returns: std::false_type {};

    template<typename Callee, typename ArgType, typename ReturnType>
    struct returns<Callee, ArgType, ReturnType, std::void_t<std::is_same<decltype(std::declval<Callee>()(std::declval<std::remove_reference_t<ArgType>>())), ReturnType>>>:std::true_type {};

    template<typename ArgType, typename Callee>
    struct call<ArgType, Callee, std::enable_if_t<isInvokeable<Callee, ArgType>::value,void>>{
        static auto with(ArgType&&arg, Callee&& callee){
            return callee(std::forward<ArgType>(arg));
        }
    };

    template<typename ArgType, typename Callee>
    struct call<ArgType, Callee, std::enable_if_t<
            !isInvokeable<Callee, ArgType>::value
            && is_optional_type<ArgType>::value
            , void>>
    {
        // piping an Optional<T> to a function void f(T), returns Optional<result_type::NothingType>
        template<typename ArgType_, typename Callee_>
        static auto with(ArgType_&&arg, Callee_&& callee)-> std::enable_if_t<std::is_void<decltype(callee(std::forward<ArgType_>(arg).value()))>::value,result_type::Optional<result_type::NothingType>>{
            if(arg) {
                callee(std::forward<ArgType_>(arg).value());
                return result_type::Optional<result_type::NothingType>(result_type::NothingType());
            }
            return result_type::Optional<result_type::NothingType>();
        }

        template<typename ArgType_, typename Callee_>
        static auto with(ArgType_&&arg, Callee_&& callee)-> std::enable_if_t<!std::is_void<decltype(callee(std::forward<ArgType_>(arg).value()))>::value,result_type::Optional<decltype(callee(std::forward<ArgType_>(arg).value()))>>{
            if(arg) {
                return callee(std::forward<ArgType_>(arg).value());
            }
            return result_type::Optional<decltype(callee(std::forward<ArgType_>(arg).value()))>();
        }
    };
}



namespace result_type {

    template<typename ArgType, typename Callee>
    constexpr auto operator|(ArgType &&arg, Callee &&callee) {
        return detail::call<ArgType, Callee>::with(std::forward<ArgType>(arg), std::forward<Callee>(callee));
    }
}
#endif //CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
