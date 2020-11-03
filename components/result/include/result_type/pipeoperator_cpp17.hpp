//
// Created by marcel on 10/24/20.
//

#ifndef CPPRESULTTYPE_PIPEOPERATOR_CPP17_HPP
#define CPPRESULTTYPE_PIPEOPERATOR_CPP17_HPP
#include <result_type/result.hpp>
#include <type_traits>
#include <result_type/typetraits.hpp>


namespace result_type {
namespace detail{
    template<typename T, typename F>
    struct CalleeIsNotInvokableWithArgType;


    template<typename OPT, typename Callee>
    constexpr auto callWithOptional(OPT&& opt, Callee&&callee){
        if constexpr (std::is_invocable_v<Callee, OPT>){
            return std::forward<Callee>(callee)(std::forward<OPT>(opt));
        }
        else if constexpr (std::is_invocable_v<Callee,  decltype(std::declval<OPT>().value())>) {
            using CalleeReturnType = decltype(std::forward<Callee>(callee)(std::forward<OPT>(opt).value()));

            if constexpr (is_optional_type<CalleeReturnType>::value) {
                if (opt.has_value()) {
                    return std::forward<Callee>(callee)(std::forward<OPT>(opt).value());
                }
                return CalleeReturnType();
            }
            else if constexpr (std::is_void_v<CalleeReturnType>){
                if (opt.has_value()) {
                    std::forward<Callee>(callee)(std::forward<OPT>(opt).value());
                    return Optional<NothingType>{NothingType{}};
                }
                else{
                    return Optional<NothingType>{};
                }
            }
            else if constexpr (is_result_type<CalleeReturnType>::value){
                using CalleeReturnSuccessType = typename CalleeReturnType::ResultSuccessType;
                if constexpr (is_optional_type<CalleeReturnSuccessType>::value){
                    using ReturnType = CalleeReturnType;
                    return (opt.has_value())
                           ? std::forward<Callee>(callee)(std::forward<OPT>(opt).value())
                           : ReturnType{CalleeReturnSuccessType{}};
                }
                else{
                    using CalleeReturnErrorType = typename CalleeReturnType::ResultErrorType;
                    using ReturnType = Result<Optional<CalleeReturnSuccessType>, CalleeReturnErrorType>;
                    if (opt.has_value()) {
                        auto result = std::forward<Callee>(callee)(std::forward<OPT>(opt).value());
                        return (isSuccess(result))
                               ? ReturnType{Optional<CalleeReturnSuccessType>(std::move(result).Success())}
                               : ReturnType{std::move(result).Error()};
                    }
                    else{
                        return ReturnType{Optional<CalleeReturnSuccessType>()};
                    }
                }
            }
            else{
                using ReturnType = Optional<CalleeReturnType>;
                if (opt.has_value()){
                    return ReturnType(std::forward<Callee>(callee)(std::forward<OPT>(opt).value()));
                }
                return ReturnType();
            }
        }
        else{
            return detail::CalleeIsNotInvokableWithArgType<decltype(std::declval<OPT>().value()), Callee>{};
        }
    }
    template<typename ArgType, typename Callee>
    constexpr auto callWithResult(ArgType &&arg, Callee&& callee) {

        using ErrorReturnType = typename std::remove_reference_t<ArgType>::ResultErrorType;
        using ArgSuccessType = typename std::remove_reference_t<ArgType>::ResultSuccessType;

        if constexpr (std::is_invocable_v<Callee, ArgType>) {
            return callee(std::forward<ArgType>(arg));
        }
        else if constexpr (std::is_invocable_v<Callee, decltype(std::forward<ArgType>(arg).Success())>) {
            using callee_return_type = decltype(callee(std::forward<ArgType>(arg).Success()));
            if constexpr (is_result_type<callee_return_type>::value) {
                return isSuccess(arg) ? callee(std::forward<ArgType>(arg).Success())
                                      : callee_return_type(std::forward<ArgType>(arg).Error());
            }
            else if constexpr (std::is_void_v<callee_return_type>) {
                using ReturnType = Result<NothingType, ErrorReturnType>;
                if (isSuccess(arg)) {
                    callee(std::forward<ArgType>(arg).Success());
                    return ReturnType(NothingType{});
                }
                return ReturnType(std::forward<ArgType>(arg).Error());
            }
            else {
                using ReturnType = Result<callee_return_type, ErrorReturnType>;
                return isSuccess(arg)
                       ? ReturnType(callee(std::forward<ArgType>(arg).Success()))
                       : ReturnType(std::forward<ArgType>(arg).Error());
            }
        }
        else if constexpr (is_optional_type<ArgSuccessType>::value){
            if constexpr (std::is_invocable_v<Callee, Result<typename ArgSuccessType::value_type, ErrorReturnType>>){
//                using callee_return_type = decltype(std::forward<Callee>(callee)(std::forward<ArgType>(arg).Success().value()));
//                using flattened_callee_return_type = decltype(callWithOptional( std::declval<ArgSuccessType>(),
//                                                                                std::forward<Callee>(callee)));
//                using ReturnType = flattened_callee_return_type;
//                if (isSuccess(arg) ) {
//                    return callWithOptional(std::forward<ArgType>(arg).Success(),
//                                                       std::forward<Callee>(callee));
//                }
//                return callee(std::forward<ArgType>(arg));
                return detail::CalleeIsNotInvokableWithArgType<ArgType, Callee>{};
            }
            else if constexpr (std::is_invocable_v<Callee, decltype (std::declval<ArgType>().Success().value())>) {
                using callee_return_type = decltype(std::forward<Callee>(callee)(std::forward<ArgType>(arg).Success().value()));

                if constexpr (is_result_type<callee_return_type>::value){
                    using CalleeReturnSuccessType = decltype(std::declval<callee_return_type>().Success());
                    if constexpr (is_optional_type<CalleeReturnSuccessType>::value){
                        using ReturnType = Result<std::remove_reference_t<CalleeReturnSuccessType>, ErrorReturnType>;

                        if (isSuccess(arg)) {
                            return (arg.CRefSuccess().has_value())
                                   ? callee(std::forward<ArgType>(arg).Success().value())
                                   : ReturnType{CalleeReturnSuccessType{}};
                        }
                        else{
                            return ReturnType(std::forward<ArgType>(arg).Error());
                        }
                    }
                    else {
                        using ReturnType = Result<Optional<std::remove_reference_t<CalleeReturnSuccessType>>, ErrorReturnType>;
                        if (isSuccess(arg)) {
                            if (arg.CRefSuccess().has_value()) {
                                auto result = callee(std::forward<ArgType>(arg).Success().value());
                                return (isSuccess(result))
                                       ? ReturnType{Optional<std::remove_reference_t<CalleeReturnSuccessType>>(std::move(result).Success())}
                                       : ReturnType{std::move(result).Error()};
                            }
                            return ReturnType{Optional<std::remove_reference_t<CalleeReturnSuccessType>>{}};

                        } else{
                            return ReturnType(std::forward<ArgType>(arg).Error());
                        }
                    }
                }
                else {
                    using flattened_callee_return_type = decltype(callWithOptional( std::forward<ArgType>(arg).Success(),
                                                                                    std::forward<Callee>(callee)));
                    using ReturnType = Result<flattened_callee_return_type, ErrorReturnType>;
                    if (isSuccess(arg)) {
                        return ReturnType{callWithOptional(std::forward<ArgType>(arg).Success(),
                                                           std::forward<Callee>(callee))};
                    }
                    return ReturnType(std::forward<ArgType>(arg).Error());
                }
            }
        }
        else {
            return detail::CalleeIsNotInvokableWithArgType<ArgType, Callee>{};
        }
    }

}
    template<typename ArgType, typename Callee>
    constexpr auto operator|(ArgType &&arg, Callee &&callee){

        if constexpr (std::is_invocable_v<Callee, ArgType>){
            return callee(std::forward<ArgType>(arg));
        }
        else if constexpr (is_result_type<ArgType>::value) {
            return detail::callWithResult(std::forward<ArgType>(arg), std::forward<Callee>(callee));
        }
        else if constexpr (is_optional_type<ArgType>::value) {
            return detail::callWithOptional(std::forward<ArgType>(arg), std::forward<Callee>(callee));
        }
        else {
            return detail::CalleeIsNotInvokableWithArgType<ArgType, Callee>{};
        }
    }

}
#endif //CPPRESULTTYPE_PIPEOPERATOR_CPP17_HPP
