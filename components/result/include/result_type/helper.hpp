//
// Created by marcel on 10/24/20.
//

#ifndef CPPRESULTTYPE_HELPER_HPP
#define CPPRESULTTYPE_HELPER_HPP
#include <result_type/typetraits.hpp>
#include <result_type/result.hpp>
#include <result_type/pipeoperator.hpp>




namespace result_type::helper{

  template<typename T, typename = void>
  struct is_not_result_and_not_option :std::false_type {};
  template<typename T>
  struct is_not_result_and_not_option<T, std::enable_if_t<!result_type::is_result_type<T>::value && !result_type::is_optional_type<T>::value, void>> :std::true_type {};


    template<typename T, typename U>
    using enable_if_is_not_result_and_not_option = std::enable_if<!result_type::is_result_type<T>::value && !result_type::is_optional_type<T>::value, U>;

    template<typename T, typename U>
    using enable_if_is_not_result_and_not_option_t = typename enable_if_is_not_result_and_not_option<T,U>::type;





}

#endif //CPPRESULTTYPE_HELPER_HPP
