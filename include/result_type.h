//
// Created by marcel on 3/31/17.
//

#ifndef RESULT_TYPE_H
#define RESULT_TYPE_H

#include <variant>

#include <typetraits.h>
#include <optional>

namespace ResultType {
    enum class ResultState {
        Success, Error
    };

    struct NothingType{
        bool operator==(NothingType const& other)const{return true;}
    };

    template<class SuccessType, class ErrorType>
    class Result {
        using ValueType = std::variant<SuccessType, ErrorType>;
        ValueType result_type_value;
    public:
        using ResultSuccessType = SuccessType;
        using ResultErrorType   = ErrorType;

        constexpr Result(Result&&) noexcept(std::is_nothrow_move_constructible_v<ValueType>) = default;
        constexpr Result& operator=(Result&&) noexcept(std::is_nothrow_move_assignable_v<ValueType>) = default;

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<std::is_copy_constructible_v<T> && !std::is_trivial_v<T>,bool>::type, typename P = SFINAE>
        constexpr explicit Result(SuccessType const &value) : result_type_value(value) {
        }

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<!std::is_trivial_v<T>,bool>::type, typename P = SFINAE>
        constexpr Result(SuccessType &&value) : result_type_value(std::move(value)) {
        }

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<std::is_trivial_v<T>,bool>::type, typename P = SFINAE>
        constexpr Result(SuccessType value) : result_type_value(value) {
        }


        constexpr Result(ErrorType &&value) : result_type_value(std::move(value)) {
        }
        constexpr Result(ErrorType const&value) : result_type_value(value) {
        }

        constexpr ResultState State() const {
            return (std::holds_alternative<SuccessType>(result_type_value))
                   ? ResultState::Success : ResultState::Error;
        }

        constexpr SuccessType &&Success() &&{
            return std::get<SuccessType>(std::move(result_type_value));
        }

        constexpr ErrorType &&Error() &&{
            return std::get<ErrorType>(std::move(result_type_value));
        }

        constexpr SuccessType &RefSuccess() &{
            return std::get<SuccessType>(result_type_value);
        }

        constexpr SuccessType const &CRefSuccess() const &{
            return std::get<SuccessType>(result_type_value);
        }

        constexpr ErrorType &RefError() &{
            return std::get<ErrorType>(result_type_value);
        }

        constexpr ErrorType const &CRefError() const &{
            return std::get<ErrorType>(result_type_value);
        }

        constexpr bool operator==(Result const &other) const {
            return other.result_type_value == result_type_value;
        }
    };

    template<typename SuccessType, typename ErrorType>
    constexpr bool IsSuccess(Result<SuccessType, ErrorType> const &result) {
        return result.State() == ResultState::Success;
    }

    template<typename SuccessType, typename ErrorType>
    constexpr bool IsError(Result<SuccessType, ErrorType> const &result) {
        return result.State() == ResultState::Error;
    }
namespace detail{

    template<typename T, typename F>
    struct CalleeIsNotInvokableWithArgType;


    template<typename OPT, typename Callee>
    constexpr auto callWithOptional(OPT&& opt, Callee&&callee){
        if constexpr (std::is_invocable_v<Callee, OPT>){
            return std::forward<Callee>(callee)(std::forward<OPT>(opt));
        }
        else if constexpr (std::is_invocable_v<Callee,  typename std::remove_reference_t<OPT>::value_type>) {
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
                    return std::optional<NothingType>{NothingType{}};
                }
                else{
                    return std::optional<NothingType>{};
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
                    using ReturnType = Result<std::optional<CalleeReturnSuccessType>, CalleeReturnErrorType>;
                    if (opt.has_value()) {
                        auto result = std::forward<Callee>(callee)(std::forward<OPT>(opt).value());
                        return (IsSuccess(result))
                                ? ReturnType{std::optional<CalleeReturnSuccessType>(std::move(result).Success())}
                                : ReturnType{std::move(result).Error()};
                    }
                    else{
                        return ReturnType{std::optional<CalleeReturnSuccessType>()};
                    }
                }
            }
            else{
                using ReturnType = std::optional<CalleeReturnType>;
                if (opt.has_value()){
                    return ReturnType(std::forward<Callee>(callee)(std::forward<OPT>(opt).value()));
                }
                return ReturnType();
            }
        }
        else{
            return detail::CalleeIsNotInvokableWithArgType<typename OPT::value_type, Callee>{};
        }
    }
    template<typename ArgType, typename Callee>
    constexpr auto callWithResult(ArgType &&arg, Callee&& callee) {

        using ErrorReturnType = typename std::remove_reference<typename ArgType::ResultErrorType>::type;
        using ArgSuccessType = typename std::remove_reference<typename ArgType::ResultSuccessType>::type;

        if constexpr (std::is_invocable_v<Callee, ArgType>) {
            return callee(std::forward<ArgType>(arg));
        }
        else if constexpr (std::is_invocable_v<Callee, ArgSuccessType>) {
            using callee_return_type = decltype(callee(std::declval<ArgSuccessType>()));
            if constexpr (is_result_type<callee_return_type>::value) {
                return IsSuccess(arg) ? callee(std::forward<ArgType>(arg).Success())
                                      : callee_return_type(std::forward<ArgType>(arg).Error());
            }
            else if constexpr (std::is_void_v<callee_return_type>) {
                using ReturnType = Result<NothingType, ErrorReturnType>;
                if (IsSuccess(arg)) {
                    callee(std::forward<ArgType>(arg).Success());
                    return ReturnType(NothingType{});
                }
                return ReturnType(std::forward<ArgType>(arg).Error());
            }
            else {
                using ReturnType = Result<callee_return_type, ErrorReturnType>;
                return IsSuccess(arg)
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
//                if (IsSuccess(arg) ) {
//                    return callWithOptional(std::forward<ArgType>(arg).Success(),
//                                                       std::forward<Callee>(callee));
//                }
//                return callee(std::forward<ArgType>(arg));
                return detail::CalleeIsNotInvokableWithArgType<ArgType, Callee>{};            }
            else if constexpr (std::is_invocable_v<Callee, typename ArgSuccessType::value_type>) {
                using callee_return_type = decltype(std::forward<Callee>(callee)(std::forward<ArgType>(arg).Success().value()));

                if constexpr (is_result_type<callee_return_type>::value){
                    using CalleeReturnSuccessType = typename callee_return_type::ResultSuccessType;
                    if constexpr (is_optional_type<CalleeReturnSuccessType>::value){
                        using ReturnType = Result<CalleeReturnSuccessType, ErrorReturnType>;

                        if (IsSuccess(arg)) {
                            return (arg.CRefSuccess().has_value())
                                   ? callee(std::forward<ArgType>(arg).Success().value())
                                   : ReturnType{CalleeReturnSuccessType{}};
                        }
                        else{
                            return ReturnType(std::forward<ArgType>(arg).Error());
                        }
                    }
                    else {
                        using ReturnType = Result<std::optional<CalleeReturnSuccessType>, ErrorReturnType>;
                        if (IsSuccess(arg)) {
                            if (arg.CRefSuccess().has_value()) {
                                auto result = callee(std::forward<ArgType>(arg).Success().value());
                                return (IsSuccess(result))
                                            ? ReturnType{std::optional<CalleeReturnSuccessType>(std::move(result).Success())}
                                            : ReturnType{std::optional<CalleeReturnSuccessType>{}};
                            }
                            return ReturnType{std::optional<CalleeReturnSuccessType>{}};

                        } else{
                            return ReturnType(std::forward<ArgType>(arg).Error());
                        }
                    }
                }
                else {
                    using flattened_callee_return_type = decltype(callWithOptional( std::declval<ArgSuccessType>(),
                                                                                    std::forward<Callee>(callee)));
                    using ReturnType = Result<flattened_callee_return_type, ErrorReturnType>;
                    if (IsSuccess(arg)) {
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
#endif //RESULT_TYPE_H
