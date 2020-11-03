//
// Created by User on 11/3/2020.
//

#ifndef CPPRESULTTYPE_APPLY_HPP
#define CPPRESULTTYPE_APPLY_HPP

#include <tuple>

#ifdef CPPRESULTTYPE_CAN_USE_STD_APPLY
namespace result_type{
    template<typename F, typename TUPLE>
    auto apply(F&&f, TUPLE&& tup) {
        return std::apply(std::forward<F>(f), std::forward<TUPLE>(tup));
    }
}
#else
//following is copied from https://en.cppreference.com/w/cpp/utility/apply
#include <functional>
namespace result_type {
    namespace detail {

        template<class F, class Tuple, std::size_t... I>
        constexpr decltype(auto) apply_impl(F &&f, Tuple &&t, std::index_sequence<I...>) {
            // This implementation is valid since C++20 (via P1065R2)
            // In C++17, a constexpr counterpart of std::invoke is actually needed here
            return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
        }
    }  // namespace detail

    template<class F, class Tuple>
    constexpr decltype(auto) apply(F &&f, Tuple &&t) {
        return detail::apply_impl(
                std::forward<F>(f), std::forward<Tuple>(t),
                std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
    }
}
#endif



#endif //CPPRESULTTYPE_APPLY_HPP
