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
    template<class T>
    using is_result_type = is_detected<detail::has_Success, T>;

    template<class SuccessType, class ErrorType>
    class Result {
        std::variant<SuccessType, ErrorType> result_type_value;
    public:
        using ResultSuccessType = SuccessType;
        using ResultErrorType   = ErrorType;
        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<std::is_copy_constructible_v<T>,bool>::type, typename P = SFINAE>
        constexpr explicit Result(SuccessType const &value) : result_type_value(value) {
        }

        constexpr Result(SuccessType &&value) : result_type_value(std::move(value)) {
        }

        constexpr Result(ErrorType &&value) : result_type_value(std::move(value)) {
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


    template<typename ArgType, typename Callee>
    constexpr auto operator|(ArgType &&arg, Callee &&f) {
        if constexpr (is_result_type<ArgType>::value) {
            using ArgSuccessType = decltype(std::forward<ArgType>(arg).Success());
            using ArgErrorType   = decltype(std::forward<ArgType>(arg).Error());

            static_assert(std::is_invocable_v<Callee, ArgType> || std::is_invocable_v<Callee, ArgSuccessType>);

            if constexpr (std::is_invocable_v<Callee, ArgType>) {
                return f(std::forward<ArgType>(arg));
            } else if constexpr (std::is_invocable_v<Callee, ArgSuccessType>) {
                using callee_return_type = decltype(f(std::declval<ArgSuccessType>()));
                if constexpr (is_result_type<callee_return_type>::value) {
                    return IsSuccess(arg) ? f(std::forward<ArgType>(arg).Success())
                                          : callee_return_type(std::forward<ArgType>(arg).Error());
                } else {
                    if constexpr (std::is_void_v<callee_return_type>) {
                        if (IsSuccess(arg)) {
                            f(std::forward<ArgType>(arg).Success());
                        }
                    } else {
                        using ReturnType = Result<callee_return_type, typename std::remove_reference<ArgErrorType>::type>;
                        return IsSuccess(arg)
                               ? ReturnType(f(std::forward<ArgType>(arg).Success()))
                               : ReturnType(std::forward<ArgType>(arg).Error());
                    }
                }
            }
        } else {
            return f(std::forward<ArgType>(arg));
        }
    }
}
#endif //RESULT_TYPE_H
