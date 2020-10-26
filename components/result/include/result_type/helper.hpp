//
// Created by marcel on 10/24/20.
//

#ifndef CPPRESULTTYPE_HELPER_HPP
#define CPPRESULTTYPE_HELPER_HPP
#include <result_type/typetraits.hpp>
#include <result_type/result.hpp>
#include <result_type/pipeoperator.hpp>


namespace result_type::helper::detail{
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

}



namespace result_type::helper{
    template<typename T, typename U>
    using enable_if_is_not_result_and_not_option = std::enable_if<!result_type::is_result_type<T>::value && !result_type::is_optional_type<T>::value, U>;

    template<typename T, typename U>
    using enable_if_is_not_result_and_not_option_t = typename enable_if_is_not_result_and_not_option<T,U>::type;


template<typename Tuple>
constexpr auto success_tuple_or_err(Tuple &&tuple) {
    static_assert(std::is_rvalue_reference<decltype(std::forward<Tuple>(tuple))>::value);
    if constexpr (detail::is_tuple_with_result<Tuple>::value) {
        using AllResultTypes = typename detail::tupleOfResultTypesIn<Tuple>::type;
        if constexpr (std::tuple_size<AllResultTypes>::value > 1) {
            static_assert(detail::have_same_error_type<AllResultTypes>::value);
        }

        auto tupleOfSuccess = [](auto &&...args) {
            return std::make_tuple(detail::successOf(std::forward<decltype(args)>(args))...);
        };
        using ReturnSuccessType = decltype(std::apply(tupleOfSuccess, std::forward<Tuple>(tuple)));
        using ReturnErrorType   = typename std::tuple_element_t<0, AllResultTypes>::ResultErrorType;

        using ReturnType = result_type::Result<ReturnSuccessType, ReturnErrorType>;

        auto is_err = [](auto const &...item) {
            return (isError(item)||...);
        };



        if (apply(is_err, std::forward<Tuple>(tuple))) {
            return ReturnType{detail::FirstError<ReturnErrorType>::get(std::forward<Tuple>(tuple)).value()};
        }
        return ReturnType{apply(tupleOfSuccess, std::forward<Tuple>(tuple))};
    } else {
        return tuple;
    }
}

template<typename T> struct wrong_type;

struct intoExitCode{
    template<typename T>
    auto operator()(T const& item)->int{
        if constexpr (is_result_type<T>::value) {
            if (isError(item)) {
                return -1;
            }
            return 0;
        }
        else if(std::is_same<T, int>::value){
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
            if(isSuccess(t)){
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
            if(isError(t)){
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

#endif //CPPRESULTTYPE_HELPER_HPP
