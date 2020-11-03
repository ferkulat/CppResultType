//
// Created by marcel on 10/26/20.
//

#ifndef CPPRESULTTYPE_SUCCESS_TUPLE_OR_ERR_HPP
#define CPPRESULTTYPE_SUCCESS_TUPLE_OR_ERR_HPP
#include <tuple>
#include <result_type/typetraits.hpp>
#include <result_type/result.hpp>
#include <result_type/apply.hpp>

namespace result_type::helper::detail{

    template <typename T, typename = void>
    struct successOfImpl{
        template<typename U>
        constexpr static auto get(U&&u){
            return std::forward<U>(u);
        }
    };

    template <typename T>
    struct successOfImpl<T, std::enable_if_t<result_type::is_result_type<T>::value, void>>{
        template<typename U>
        constexpr static auto get(U&&u){
            return std::forward<U>(u).Success();
        }
    };

    template<typename T>
    auto successOf(T &&t) {
        return successOfImpl<T>::get(std::forward<T>(t));
//        if constexpr (result_type::is_result_type<T>::value) {
//            return std::forward<T>(t).Success();
//        } else {
//            return std::move(t);
//        }
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
        constexpr static bool value = std::conjunction<std::is_same<typename ResType::ResultErrorType, typename ResTypes::ResultErrorType>...>::value;
    };

    template<typename ErrorType>
    struct FirstError {
    private:
        template<std::size_t>
        struct tuple_element_index {
        };

        template<typename Tuple>
        constexpr static
        std::enable_if_t<is_result_type<std::tuple_element_t<0, Tuple>>::value, Optional<ErrorType>>
        getImpl(Tuple &&tuple, tuple_element_index<0>) {
            return (result_type::isError(std::get<0>(tuple)))
                   ? std::get<0>(std::forward<Tuple>(tuple)).Error()
                   : Optional<ErrorType>();
        }

        template<size_t N, typename Tuple>
        constexpr static
        std::enable_if_t<is_result_type<std::tuple_element_t<N, Tuple>>::value, Optional<ErrorType>>
        getImpl(Tuple &&tuple,
                tuple_element_index<N>) {

            return (result_type::isError(std::get<N>(tuple)))
                   ? std::get<N>(std::forward<Tuple>(tuple)).Error()
                   : getImpl(std::forward<Tuple>(tuple), tuple_element_index<N - 1>());
        }

        template<typename Tuple>
        constexpr static
        std::enable_if_t<!is_result_type<std::tuple_element_t<0, Tuple>>::value, Optional<ErrorType>>
        getImpl(Tuple &&, tuple_element_index<0>) {
            return Optional<ErrorType>();
        }

        template<size_t N, typename Tuple>
        constexpr static
        std::enable_if_t<!is_result_type<std::tuple_element_t<N, Tuple>>::value, Optional<ErrorType>>
        getImpl(Tuple &&tuple, tuple_element_index<N>) {
            return getImpl(std::forward<Tuple>(tuple), tuple_element_index<N - 1>());
        }

    public:
        template<typename Tuple>
        constexpr static auto get(Tuple &&tuple) -> Optional<ErrorType> {
            return getImpl(std::forward<Tuple>(tuple), tuple_element_index<std::tuple_size<Tuple>::value - 1>());
        }
    };

    template<typename Tuple, typename = void>
    struct success_tuple_or_err_impl;

    template<typename Tuple>
    struct success_tuple_or_err_impl<Tuple, std::enable_if_t<
            is_tuple_with_result<Tuple>::value, void>>{
        template<typename T>
        constexpr static auto get(T&&tuple){
            using AllResultTypes = typename tupleOfResultTypesIn<Tuple>::type;
            static_assert(have_same_error_type<AllResultTypes>::value);

            auto tupleOfSuccess = [](auto &&...args) {
                return std::make_tuple(successOf(std::forward<decltype(args)>(args))...);
            };
            using ReturnSuccessType = decltype(apply(tupleOfSuccess, std::forward<Tuple>(tuple)));
            using ReturnErrorType   = typename std::tuple_element_t<0, AllResultTypes>::ResultErrorType;

            using ReturnType = result_type::Result<ReturnSuccessType, ReturnErrorType>;

            auto is_err = [](auto const &...item) {
                return (isError(item)||...);
            };



            if (apply(is_err, std::forward<Tuple>(tuple))) {
                return ReturnType{FirstError<ReturnErrorType>::get(std::forward<Tuple>(tuple)).value()};
            }
            return ReturnType{apply(tupleOfSuccess, std::forward<Tuple>(tuple))};
        }
    };
    template<typename Tuple>
    struct success_tuple_or_err_impl<Tuple, std::enable_if_t<
            !is_tuple_with_result<Tuple>::value, void>>{
        template<typename T>
        constexpr static auto get(T&&tuple) {
            return std::forward<Tuple>(tuple);
        }};


        }

namespace result_type::helper{
    template<typename Tuple>
    constexpr auto success_tuple_or_err(Tuple &&tuple) {
        static_assert(std::is_rvalue_reference<decltype(std::forward<Tuple>(tuple))>::value);
        return detail::success_tuple_or_err_impl<Tuple>::get(std::forward<Tuple>(tuple));
    }

}
#endif //CPPRESULTTYPE_SUCCESS_TUPLE_OR_ERR_HPP
