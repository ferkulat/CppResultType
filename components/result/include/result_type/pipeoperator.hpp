//
// Created by User on 10/24/2020.
//

#ifndef CPPRESULTTYPE_PIPEOPERATOR_H
#define CPPRESULTTYPE_PIPEOPERATOR_H

#ifdef CPPRESULTTYPE_WITH_CPP14_PIPE
#include <boost/optional.hpp>
namespace result_type {
    template<typename T>
    using Optional = boost::optional<T>;

}
#include <result_type/pipeoperator_cpp14.hpp>
#else
namespace result_type {
    template<typename T>
    using Optional = std::optional<T>;

}
#include <result_type/pipeoperator_cpp17.hpp>
#endif
#endif //CPPRESULTTYPE_PIPEOPERATOR_H
