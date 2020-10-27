//
// Created by User on 10/24/2020.
//

#ifndef CPPRESULTTYPE_PIPEOPERATOR_H
#define CPPRESULTTYPE_PIPEOPERATOR_H
#include <result_type/optional.hpp>
#ifdef CPPRESULTTYPE_WITH_CPP14_PIPE
#include <result_type/pipeoperator_cpp14.hpp>
#else
#include <result_type/pipeoperator_cpp17.hpp>
#endif
#endif //CPPRESULTTYPE_PIPEOPERATOR_H
