//
// Created by User on 10/27/2020.
//

#ifndef CPPRESULTTYPE_APPLY_HPP
#define CPPRESULTTYPE_APPLY_HPP
#include <tuple>
#include <utility>
#include <type_traits>
#include <functional>
namespace result_type::helper::detail{
// that does not work on gcc6 yet


    //    namespace detail {
//        template <class F, class Tuple, std::size_t... I>
//        constexpr auto apply_impl( F&& f, Tuple&& t, std::index_sequence<I...> )
//        {
//            return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
//            // Note: std::invoke is a C++17 feature
//        }
//    } // namespace detail
//
//    template <class F, class Tuple>
//    constexpr auto apply(F&& f, Tuple&& t)
//    {
//        return detail::apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
//                                  std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
//    }
}
#endif //CPPRESULTTYPE_APPLY_HPP
