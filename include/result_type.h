//
// Created by marcel on 3/31/17.
//

#ifndef RESULT_TYPE_H
#define RESULT_TYPE_H

#include <variant>

#include <typetraits.h>

namespace ResultType {
    namespace detail {

        template<class T>
        using has_Success = decltype(std::declval<T>().Success());

    }
    enum class ResultState {
        Success, Error
    };

    template<class SuccessType, class ErrorType>
    class Result {
        std::variant<SuccessType, ErrorType> result_type_value;
    public:
        constexpr explicit Result(SuccessType const &value) : result_type_value(value) {
        }

        constexpr explicit Result(SuccessType &&value) : result_type_value(std::move(value)) {
        }

        constexpr explicit Result(ErrorType &&value) : result_type_value(std::move(value)) {
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

        constexpr SuccessType const& CRefSuccess() const&{
            return std::get<SuccessType>(result_type_value);
        }

        constexpr ErrorType &RefError() &{
            return std::get<ErrorType>(result_type_value);
        }
        constexpr ErrorType const& CRefError() const&{
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

    template<typename T, typename F>
    constexpr auto operator|(T &&t, F f) {
        return f(std::forward<T>(t));
    }

    template<typename T, typename E, typename Callee>
    constexpr auto operator|(Result<T, E> &&result, Callee &&f) {
        using ArgType = Result<T, E>;
        using ArgSuccessType = decltype(std::forward<Result<T, E>>(result).Success());

        if constexpr (std::is_invocable_v<Callee, Result<T, E>>){
            using callee_return_type = decltype(f(std::declval<ArgType>()));
            return f(std::forward<Result<T, E>>(result));
        }
        else if constexpr (std::is_invocable_v<Callee, T>){
            using callee_return_type = decltype(f(std::declval<ArgSuccessType>()));
            if constexpr (is_detected<detail::has_Success, callee_return_type>::value) {
                return IsSuccess(result) ? f(result.Success())
                                         : callee_return_type(result.Error());
            }
            else{
                return IsSuccess(result) ? Result<callee_return_type, E>(f(std::forward<Result<T, E>>(result).Success()))
                                         : Result<callee_return_type, E>(std::forward<Result<T, E>>(result).Error());
            }
        }
//        if constexpr(is_detected<detail::has_Success, callee_return_type>::value) {
//            return IsSuccess(result) ? f(std::forward<Result<T, E>>(result))
//                                     : callee_return_type(result.Error());
//        }
//        else if constexpr(std::is_void_v<callee_return_type>){
//            f(std::forward<Result<T, E>>(result));
//        }
//        return Result<callee_return_type, E>(f(std::forward<Result<T, E>>(result)));
    }
}
#endif //RESULT_TYPE_H
