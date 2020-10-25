//
// Created by User on 10/25/2020.
//

#ifndef CPPRESULTTYPE_VALUE_TYPE_OF_HPP
#define CPPRESULTTYPE_VALUE_TYPE_OF_HPP
#include <result_type/optional.hpp>
#include <result_type/result.hpp>

namespace result_type::detail {
    template<typename ValueType, typename... ErrorType>
    struct value_type_of {
        using type = ValueType;
    };

    template<typename S>
    struct value_type_of<Optional<S>> {
        using type = S;
    };

    template<typename S, typename... E>
    struct value_type_of<result_type::Result<S, E...>> {
        using type = typename value_type_of<S>::type;
    };
    template<typename T>
    using value_type_of_t = typename value_type_of<T>::type;
}
#endif //CPPRESULTTYPE_VALUE_TYPE_OF_HPP
