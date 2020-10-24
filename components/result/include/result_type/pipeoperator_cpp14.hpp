//
// Created by User on 10/24/2020.
//

#ifndef CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
#define CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
#include <type_traits>
#include <result_type/result.hpp>
#include <type_traits>
namespace result_type {

  template<typename ArgType, typename Callee>
  constexpr auto operator|(ArgType &&arg, Callee &&callee) {

      return callee(std::forward<ArgType>(arg));
  }
}
#endif //CPPRESULTTYPE_PIPEOPERATOR_CPP14_H
