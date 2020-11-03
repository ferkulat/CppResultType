//
// Created by marcel on 4/15/19.
//

#ifndef CSV2XLS_TYPETRAITS_H
#define CSV2XLS_TYPETRAITS_H

#include <type_traits>
#include <result_type/optional.hpp>
namespace result_type {
namespace detail {
    template <class Default, class AlwaysVoid,
              template<class...> class Op, class... Args>
    struct detector
    {
        using value_t = std::false_type;
        using type = Default;
    };

    template <class Default, template<class...> class Op, class... Args>
    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
    {
        using value_t = std::true_type;
        using type = Op<Args...>;
    };

} // namespace detail

    namespace detail {

        template<class T>
        using has_Success = decltype(std::declval<T>().Success());

        template<class T>
        using has_method_has_value = decltype(std::declval<std::remove_reference_t<T>>().has_value());

        template<class T>
        using has_method_is_initialized = decltype(std::declval<std::remove_reference_t<T>>().is_initialized());

        template<class T>
        using has_method_value_or = decltype(std::declval<std::remove_reference_t<T>>().value_or(std::declval<typename std::remove_reference_t<T>::value_type>()));
    }

// special type to indicate detection failure
struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

template <template<class...> class Op, class... Args>
using is_detected =
    typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template<class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

    template<class T, typename = void>
    struct is_result_type:std::false_type{};

    template<class T>
    struct is_result_type<T, std::void_t<detail::has_Success<T>>>:std::true_type{};

    template<class T>
    using has_method_has_value = is_detected<detail::has_method_has_value, T>;

    template<class T>
    using has_method_value_or = is_detected<detail::has_method_value_or, T>;

    template<typename T, typename = void>
    struct is_std_optional_type : std::false_type {};

    template<typename T>
    struct is_std_optional_type<T,
            std::void_t< detail::has_method_has_value<T>
                        ,detail::has_method_value_or<T>
            >
    > : std::true_type {};

    template<typename T, typename = void>
    struct is_boost_optional_type : std::false_type {};
    
    template<typename T>
    struct is_boost_optional_type<T,
        std::void_t< detail::has_method_is_initialized<T>
                    ,detail::has_method_value_or<T>
        >
    > : std::true_type {};

    template<typename T, typename = void>
    struct is_optional_type : std::false_type {};

    template<typename T>
    struct is_optional_type<T,
            std::enable_if_t<
                    is_std_optional_type<T>::value || is_boost_optional_type<T>::value
            , void>
    > : std::true_type {};


    template <typename T, typename = void>
    struct isResultTypeWithNonOptional:std::false_type {};
    template <typename T>
    struct isResultTypeWithNonOptional<T, std::enable_if_t<
            is_result_type<T>::value
            && !is_optional_type<decltype(std::declval<T>().Success())>::value
            , void>>:std::true_type {};
    template <typename T, typename = void>
    struct isResultTypeWithOptional:std::false_type {};
    template <typename T>
    struct isResultTypeWithOptional<T, std::enable_if_t<
            is_result_type<T>::value
            && is_optional_type<decltype(std::declval<T>().Success())>::value
            , void>>:std::true_type {};


    template<typename Callee, typename ArgType>
    using accepts = decltype(std::declval<Callee>()(std::declval<ArgType>()));

    template<typename Callee, typename ArgType, typename = void>
    struct isInvokeable : std::false_type{};

    template<typename Callee, typename ArgType>
    struct isInvokeable<Callee, ArgType, std::void_t<accepts<Callee, ArgType>>>:std::true_type {};

    template<typename...Ts>
    struct is_tuple_with_result : std::false_type {
    };

    template<typename...Ts>
    struct is_tuple_with_result<std::tuple<Ts...>> {
        constexpr static bool value = (result_type::is_result_type<Ts>::value || ...);
    };

    template<typename ArgType, typename CalleeType, typename = void>
    struct ArgumentIsResultWithNonOptionalButFunctionAcceptsItsSuccessType: std::false_type{};

    template<typename ArgType, typename CalleeType>
    struct ArgumentIsResultWithNonOptionalButFunctionAcceptsItsSuccessType<ArgType, CalleeType,
            std::enable_if_t<!isInvokeable<CalleeType, ArgType>::value
            && isResultTypeWithNonOptional<ArgType>::value
            && isInvokeable<CalleeType, decltype(std::declval<ArgType>().Success())>::value, void>>:std::true_type {};


}
#endif //CSV2XLS_TYPETRAITS_H
