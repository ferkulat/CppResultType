//
// Created by marcel on 10/26/20.
//

#ifndef CPPRESULTTYPE_SUCCESS_TYPE_HPP
#define CPPRESULTTYPE_SUCCESS_TYPE_HPP

#include <result_type/result.hpp>
namespace result_type::helper::detail{
    template<typename S>
    struct success_type {
        using type = S;
    };

    template<typename S, typename E>
    struct success_type<result_type::Result<S, E>> {
        using type = S;
    };

}

#endif //CPPRESULTTYPE_SUCCESS_TYPE_HPP
