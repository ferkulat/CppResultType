//
// Created by marcel on 3/31/17.
//

#ifndef RESULT_TYPE_H
#define RESULT_TYPE_H

#include <result_type/variant.hpp>

#include <result_type/typetraits.hpp>
#include <tuple>
#include <type_traits>

namespace result_type {
    enum class ResultState {
        Success, Error
    };
    template<class T>
    using has_method_which = decltype(std::declval<std::remove_reference_t<T>>().which());
    template<class T, typename = void>
    struct is_boost_variant: std::false_type {};


    template<class T>
    struct is_boost_variant<T,std::void_t<has_method_which<T>>>:std::true_type {};

    struct NothingType{
        bool operator==(NothingType const& )const{return true;}
    };

    template<class SuccessType, class ErrorType>
    class Result {
        using ValueType = result_type::Variant<SuccessType, ErrorType>;
        ValueType result_type_value;
    public:
        using ResultSuccessType = SuccessType;
        using ResultErrorType   = ErrorType;

//        constexpr Result(Result&&) noexcept(std::is_nothrow_move_constructible_v<ValueType>) = default;
//        constexpr Result& operator=(Result&&) noexcept(std::is_nothrow_move_assignable_v<ValueType>) = default;
//        constexpr Result(Result const&) noexcept(std::is_nothrow_copy_constructible_v<ValueType>) = default;
//        constexpr Result& operator=(Result const&) noexcept(std::is_nothrow_copy_assignable_v<ValueType>) = default;

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<std::is_copy_constructible<T>::value && !std::is_trivial<T>::value,bool>::type, typename P = SFINAE>
        constexpr Result(SuccessType const &value) : result_type_value(value) {
        }

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<!std::is_trivial<T>::value,bool>::type, typename P = SFINAE>
        constexpr Result(SuccessType &&value) : result_type_value(std::move(value)) {
        }

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<std::is_trivial<T>::value,bool>::type, typename P = SFINAE>
        constexpr Result(SuccessType value) : result_type_value(value) {
        }


        constexpr Result(ErrorType &&value) : result_type_value(std::move(value)) {
        }
        constexpr Result(ErrorType const&value) : result_type_value(value) {
        }

        template<typename U = ValueType>
        constexpr auto State() const -> std::enable_if_t<!is_boost_variant<U>::value, ResultState>{
            return (result_type_value.index() == 0)
                   ? ResultState::Success : ResultState::Error;
        }
        template<typename U = ValueType>
        constexpr auto State() const -> std::enable_if_t<is_boost_variant<U>::value, ResultState>{
            return (result_type_value.which() == 0)
                   ? ResultState::Success : ResultState::Error;
        }

        constexpr SuccessType &&Success() &&{
            return CPPRESULTTYPE_VARIANT_GET<SuccessType>(std::move(result_type_value));
        }
        constexpr SuccessType& Success() &{
            return CPPRESULTTYPE_VARIANT_GET<SuccessType>(result_type_value);
        }
        constexpr SuccessType const& Success() const&{
            return CPPRESULTTYPE_VARIANT_GET<SuccessType>(result_type_value);
        }

        constexpr ErrorType &&Error() &&{
            return CPPRESULTTYPE_VARIANT_GET<ErrorType>(std::move(result_type_value));
        }
        constexpr ErrorType& Error() &{
            return CPPRESULTTYPE_VARIANT_GET<ErrorType>(result_type_value);
        }
        constexpr ErrorType const& Error() const&{
            return CPPRESULTTYPE_VARIANT_GET<ErrorType>(result_type_value);
        }

        constexpr SuccessType &RefSuccess() &{
            return CPPRESULTTYPE_VARIANT_GET<SuccessType>(result_type_value);
        }

        constexpr SuccessType const &CRefSuccess() const &{
            return CPPRESULTTYPE_VARIANT_GET<SuccessType>(result_type_value);
        }

        constexpr ErrorType &RefError() &{
            return CPPRESULTTYPE_VARIANT_GET<ErrorType>(result_type_value);
        }

        constexpr ErrorType const &CRefError() const &{
            return CPPRESULTTYPE_VARIANT_GET<ErrorType>(result_type_value);
        }

        constexpr bool operator==(Result const &other) const {
            return other.result_type_value == result_type_value;
        }
    };

    template<typename T>
    constexpr auto IsSuccess(T const &result)->std::enable_if_t< is_result_type<T>::value ,bool> {
        return result.State() == ResultState::Success;
    }
    template<typename T>
    constexpr auto IsSuccess(T const &)->std::enable_if_t< !is_result_type<T>::value ,bool> {
        return true;
    }

    template<typename T>
    constexpr bool IsError(T const &result) {
        return ! IsSuccess(result);
    }
}
#endif //RESULT_TYPE_H
