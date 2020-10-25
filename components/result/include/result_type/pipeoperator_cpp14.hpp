//
// Created by User on 10/24/2020.
//

#ifndef CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
#define CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
#include <type_traits>
#include <result_type/result.hpp>
#include <result_type/typetraits.hpp>
#include <result_type/detail/returntype.hpp>
namespace result_type {

    template<typename ArgType, typename Callee>
    constexpr auto operator|(ArgType &&arg, Callee &&callee);
}

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

        template<typename T>
        struct intoOptional{
            auto operator()(T&&t){
                return Optional<T>(std::forward<T>(t));
            }
        };


    template<typename ArgType, typename Callee>
    struct call<ArgType, Callee, std::enable_if_t<
            !isInvokeable<Callee, ArgType>::value
            && is_optional_type<ArgType>::value
            , void>>
    {
        // piping an Optional<T> to a function void f(T), returns Optional<result_type::NothingType>
        template<typename OptArgType, typename Callee_>
        static auto with(OptArgType&&opt_arg, Callee_&& callee)-> std::enable_if_t<std::is_void<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value,result_type::Optional<result_type::NothingType>>{
            if(opt_arg) {
                callee(std::forward<OptArgType>(opt_arg).value());
                return result_type::Optional<result_type::NothingType>(result_type::NothingType());
            }
            return result_type::Optional<result_type::NothingType>();
        }

        template<typename OptArgType, typename Callee_>
        static auto with(OptArgType&&opt_arg, Callee_&& callee)-> std::enable_if_t<
                !std::is_void<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
                && !is_optional_type<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
                && !is_result_type<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
        ,result_type::Optional<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>>{
            if(opt_arg) {
                return callee(std::forward<OptArgType>(opt_arg).value());
            }
            return result_type::Optional<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>();
        }

        template<typename OptArgType, typename Callee_>
        static auto with(OptArgType&&opt_arg, Callee_&& callee)-> std::enable_if_t<
                !std::is_void<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
                && is_optional_type<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
        ,decltype(callee(std::forward<OptArgType>(opt_arg).value()))>{
            if(opt_arg) {
                return callee(std::forward<OptArgType>(opt_arg).value());
            }
            return decltype(callee(std::forward<OptArgType>(opt_arg).value()))();
        }

        template<typename OptArgType, typename Callee_>
        static auto with(OptArgType&&opt_arg, Callee_&& callee)-> std::enable_if_t<
                isResultTypeWithNonOptional<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
                ,ReturnType_t<OptArgType, decltype(callee(std::forward<OptArgType>(opt_arg).value()))>>{
            using CalleeReturnType = decltype(callee(std::forward<OptArgType>(opt_arg).value()));
            using CalleeSuccesType = typename CalleeReturnType::ResultSuccessType;
            if(opt_arg){
                using result_type::operator|;
                return std::forward<OptArgType>(opt_arg).value()
                        | std::forward<Callee_>(callee)
                        | intoOptional<CalleeSuccesType>{}
                        ;
            }
            using ReturnType = ReturnType_t<OptArgType, decltype(callee(std::forward<OptArgType>(opt_arg).value()))>;
            return ReturnType(Optional<CalleeSuccesType>());
        }

        template<typename OptArgType, typename Callee_>
        static auto with(OptArgType&&opt_arg, Callee_&& callee)-> std::enable_if_t<
                isResultTypeWithOptional<decltype(callee(std::forward<OptArgType>(opt_arg).value()))>::value
                ,ReturnType_t<OptArgType, decltype(callee(std::forward<OptArgType>(opt_arg).value()))>>{
            if(opt_arg){
                using result_type::operator|;
                return std::forward<OptArgType>(opt_arg).value()
                        | std::forward<Callee_>(callee)
                        ;
            }
            using CalleeReturnType = decltype(callee(std::forward<OptArgType>(opt_arg).value()));
            using CalleeSuccesType = typename CalleeReturnType::ResultSuccessType;
            return CalleeSuccesType();
        }
    };

    template<typename ArgType, typename Callee>
    struct call<ArgType, Callee, std::enable_if_t<
            !isInvokeable<Callee, ArgType>::value
            && is_result_type<ArgType>::value
            , void>>
    {
        // piping a Result<T, E> to a function void f(T), returns Result<result_type::NothingType, E>
        template<typename ResultArgType, typename Callee_>
        static auto with(ResultArgType&&result_arg, Callee_&& callee)-> std::enable_if_t<std::is_void<decltype(callee(std::forward<ResultArgType>(result_arg).Success()))>::value
        ,detail::ReturnType_t<ResultArgType, decltype(callee(std::forward<ResultArgType>(result_arg).Success())) >>{
            if (IsSuccess(result_arg)){
                callee(std::forward<ResultArgType>(result_arg).Success());
                return NothingType();
            }
            return std::forward<ResultArgType>(result_arg).Error();
        }
        // piping a Result<T, E> to a function  f(T)->U, returns Result<U, E>
        template<typename ResultArgType, typename Callee_>
        static auto with(ResultArgType&&result_arg, Callee_&& callee)-> std::enable_if_t<
                !std::is_void<decltype(callee(std::forward<ResultArgType>(result_arg).Success()))>::value
                && !is_result_type<decltype(callee(std::forward<ResultArgType>(result_arg).Success()))>::value
                ,detail::ReturnType_t<ResultArgType, decltype(callee(std::forward<ResultArgType>(result_arg).Success())) >>{
            if (IsSuccess(result_arg)){
                return callee(std::forward<ResultArgType>(result_arg).Success());
            }
            return std::forward<ResultArgType>(result_arg).Error();
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
