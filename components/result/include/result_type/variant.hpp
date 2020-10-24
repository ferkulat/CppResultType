//
// Created by User on 10/26/2020.
//

#ifndef CPPRESULTTYPE_VARIANT_HPP
#define CPPRESULTTYPE_VARIANT_HPP

#ifdef CPPRESULTTYPE_WITH_BOOST_VARIANT
#include <boost/variant.hpp>
namespace result_type{
  template<typename...T>
  using Variant = boost::variant<T...>;

#define CPPRESULTTYPE_VARIANT_GET boost::strict_get
}
#else
#include <variant>
namespace result_type{
  template<typename...T>
  using Variant = std::variant<T...>;
#define CPPRESULTTYPE_VARIANT_GET std::get
}
#endif

#endif //CPPRESULTTYPE_VARIANT_HPP
