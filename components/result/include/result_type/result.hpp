//
// Created by marcel on 3/31/17.
//

#ifndef RESULT_TYPE_H
#define RESULT_TYPE_H

#include <variant>

#include <result_type/typetraits.hpp>
#include <optional>
#include <tuple>
#include <type_traits>

namespace result_type {
    enum class ResultState {
        Success, Error
    };

    struct NothingType{
        bool operator==(NothingType const& )const{return true;}
    };

    template<class SuccessType, class ErrorType>
    class Result {
        using ValueType = std::variant<SuccessType, ErrorType>;
        ValueType result_type_value;
    public:
        using ResultSuccessType = SuccessType;
        using ResultErrorType   = ErrorType;

//        constexpr Result(Result&&) noexcept(std::is_nothrow_move_constructible_v<ValueType>) = default;
//        constexpr Result& operator=(Result&&) noexcept(std::is_nothrow_move_assignable_v<ValueType>) = default;
//        constexpr Result(Result const&) noexcept(std::is_nothrow_copy_constructible_v<ValueType>) = default;
//        constexpr Result& operator=(Result const&) noexcept(std::is_nothrow_copy_assignable_v<ValueType>) = default;

        template<typename T = SuccessType, typename SFINAE = typename std::enable_if<std::is_copy_constructible_v<T> && !std::is_trivial_v<T>,bool>::type, typename P = SFINAE>
        constexpr Result(SuccessType const &value) : result_type_value(value) {
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
        constexpr SuccessType& Success() &{
            return std::get<SuccessType>(result_type_value);
        }
        constexpr SuccessType const& Success() const&{
            return std::get<SuccessType>(result_type_value);
        }

        constexpr ErrorType &&Error() &&{
            return std::get<ErrorType>(std::move(result_type_value));
        }
        constexpr ErrorType& Error() &{
            return std::get<ErrorType>(result_type_value);
        }
        constexpr ErrorType const& Error() const&{
            return std::get<ErrorType>(result_type_value);
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
    template<typename...Ts>
    struct is_tuple_with_result : std::false_type {
    };

    template<typename...Ts>
    struct is_tuple_with_result<std::tuple<Ts...>> {
        constexpr static auto value = (result_type::is_result_type<Ts>::value || ...);
    };

    template<typename S>
    struct success_type {
        using type = S;
    };

    template<typename S, typename E>
    struct success_type<result_type::Result<S, E>> {
        using type = S;
    };

    template<typename T>
    bool isError([[maybe_unused]] T const &t) {
        if constexpr (result_type::is_result_type<T>::value) {
            return result_type::IsError(t);
        } else {
            return false;
        }
    }

    template<typename T>
    auto successOf(T &&t) {
        if constexpr (result_type::is_result_type<T>::value) {
            return std::forward<T>(t).Success();
        } else {
            return std::move(t);
        }
    }

    template<typename T, typename...Ts>
    struct appendIfResultType;

    template<typename T, typename...Ts>
    struct appendIfResultType<T, std::tuple<Ts...>> {
        using type = std::tuple<Ts...>;
    };

    template<typename S, typename E, typename...Ts>
    struct appendIfResultType<result_type::Result<S, E>, std::tuple<Ts...>> {
        using type = std::tuple<Ts..., result_type::Result<S, E>>;
    };

    template<size_t tailsize, typename Tuple, typename Head, typename...Tail>
    struct tupleOfResultTypesInImpl {
        using type = typename tupleOfResultTypesInImpl<
                sizeof...(Tail) - 1, typename appendIfResultType<Head, Tuple>::type, Tail...>::type;
    };

    template<typename Tuple, typename Head, typename...Tail>
    struct tupleOfResultTypesInImpl<0, Tuple, Head, Tail...> {
        using type =  typename appendIfResultType<Head, Tuple>::type;
    };

    template<typename...T>
    struct tupleOfResultTypesIn;

    template<typename...T>
    struct tupleOfResultTypesIn<std::tuple<T...>> {
        using type = typename tupleOfResultTypesInImpl<sizeof...(T) - 1, std::tuple<>, T...>::type;
    };

    template<typename ResType, typename... ResTypes>
    struct have_same_error_type;

    template<typename ResType, typename... ResTypes>
    struct have_same_error_type<std::tuple<ResType, ResTypes...>> {
        constexpr static bool value = std::conjunction_v<std::is_same<typename ResType::ResultErrorType, typename ResTypes::ResultErrorType>...>;
    };

    template<typename ErrorType>
    struct FirstError {
    private:
        template<std::size_t>
        struct tuple_element_index {
        };

        template<typename Tuple>
        constexpr static
        std::enable_if_t<is_result_type<std::tuple_element_t<0, Tuple>>::value, std::optional<ErrorType>>
        getImpl(Tuple &&tuple, tuple_element_index<0>) {
            return (result_type::IsError(std::get<0>(tuple)))
                   ? std::get<0>(std::forward<Tuple>(tuple)).Error()
                   : std::optional<ErrorType>();
        }

        template<size_t N, typename Tuple>
        constexpr static
        std::enable_if_t<is_result_type<std::tuple_element_t<N, Tuple>>::value, std::optional<ErrorType>>
        getImpl(Tuple &&tuple,
                tuple_element_index<N>) {

            return (result_type::IsError(std::get<N>(tuple)))
                   ? std::get<N>(std::forward<Tuple>(tuple)).Error()
                   : getImpl(std::forward<Tuple>(tuple), tuple_element_index<N - 1>());
        }

        template<typename Tuple>
        constexpr static
        std::enable_if_t<!is_result_type<std::tuple_element_t<0, Tuple>>::value, std::optional<ErrorType>>
        getImpl(Tuple &&, tuple_element_index<0>) {
            return std::optional<ErrorType>();
        }

        template<size_t N, typename Tuple>
        constexpr static
        std::enable_if_t<!is_result_type<std::tuple_element_t<N, Tuple>>::value, std::optional<ErrorType>>
        getImpl(Tuple &&tuple, tuple_element_index<N>) {
            return getImpl(std::forward<Tuple>(tuple), tuple_element_index<N - 1>());
        }

    public:
        template<typename Tuple>
        constexpr static auto get(Tuple &&tuple) -> std::optional<ErrorType> {
            return getImpl(std::forward<Tuple>(tuple), tuple_element_index<std::tuple_size_v<Tuple> - 1>());
        }
    };


}

    namespace helper{
        template<typename T, typename U>
        using enable_if_is_not_result_and_not_option = std::enable_if<!result_type::is_result_type<T>::value && !result_type::is_optional_type<T>::value, U>;

        template<typename T, typename U>
        using enable_if_is_not_result_and_not_option_t = typename enable_if_is_not_result_and_not_option<T,U>::type;

        template<typename ValueType, typename... ErrorType> struct value_type_of{
            using type = ValueType;
        };

        template<typename S> struct value_type_of<std::optional<S>>{
            using type = S;
        };

        template<typename S, typename... E> struct value_type_of<result_type::Result<S,E...>>{
            using type = typename value_type_of<S>::type;
        };
        template <typename T>
        using value_type_of_t = typename value_type_of<T>::type;

        template<typename Tuple>
        constexpr auto success_tuple_or_err(Tuple &&tuple) {
            static_assert(std::is_rvalue_reference_v<decltype(std::forward<Tuple>(tuple))>);
            if constexpr (detail::is_tuple_with_result<Tuple>::value) {
                using AllResultTypes = typename detail::tupleOfResultTypesIn<Tuple>::type;
                if constexpr (std::tuple_size_v<AllResultTypes> > 1) {
                    static_assert(detail::have_same_error_type<AllResultTypes>::value);
                }

                auto tupleOfSuccess = [](auto &&...args) {
                    return std::make_tuple(detail::successOf(std::forward<decltype(args)>(args))...);
                };
                using ReturnSuccessType = decltype(std::apply(tupleOfSuccess, std::forward<Tuple>(tuple)));
                using ReturnErrorType   = typename std::tuple_element_t<0, AllResultTypes>::ResultErrorType;

                using ReturnType = result_type::Result<ReturnSuccessType, ReturnErrorType>;

                auto is_err = [](auto const &...item) {
                    return (detail::isError(item)||...);
                };



                if (std::apply(is_err, std::forward<Tuple>(tuple))) {
                    return ReturnType{detail::FirstError<ReturnErrorType>::get(std::forward<Tuple>(tuple)).value()};
                }
                return ReturnType{std::apply(tupleOfSuccess, std::forward<Tuple>(tuple))};
            } else {
                return tuple;
            }
        }

        template<typename T> struct wrong_type;

        struct intoExitCode{
            template<typename T>
            auto operator()(T const& item)->int{
                if constexpr (is_result_type<T>::value) {
                    if (IsError(item)) {
                        return -1;
                    }
                    return 0;
                }
                else if(std::is_same_v<T, int>){
                    return item;
                }
                else{
                    wrong_type<T>{};
                }
            }
        };
        namespace helper_detail{

            template<typename OStreamType, typename T>
            void streamSuccessHelper(OStreamType& os, T const& t){
                if constexpr (is_result_type<T>::value){
                    if(IsSuccess(t)){
                        streamSuccessHelper(os, t.CRefSuccess());
                    }
                }
                else if constexpr (is_optional_type<T>::value){
                    if(t.has_value()) {
                        streamSuccessHelper(os, t.value());
                    }
                }
                else{
                    os << t;
                }
            }

            template<typename OStreamType, typename T>
            void streamErrorHelper(OStreamType& os, T const& t){
                if constexpr (is_result_type<T>::value){
                    if(IsError(t)){
                        os << t.CRefError();
                    }
                }
            }

        }

        template<typename OStreamType>
        auto streamSuccessTo(OStreamType& os){
            return [&os](auto&& item){
                helper_detail::streamSuccessHelper(os, item);
                return std::forward<decltype(item)>(item);
            };
        }

        template<typename OStreamType>
        auto streamErrorTo(OStreamType& os){
            return [&os](auto&& item){
                helper_detail::streamErrorHelper(os, item);
                return std::forward<decltype(item)>(item);
            };
        }


    }
}
#endif //RESULT_TYPE_H
