//
// Created by User on 10/25/2020.
//

#ifndef CPPRESULTTYPE_OPTIONAL_HPP
#define CPPRESULTTYPE_OPTIONAL_HPP


#ifdef CPPRESULTTYPE_WITH_CPP14_PIPE

#include <boost/optional.hpp>
namespace result_type {
    template<typename T>
    using Optional = boost::optional<T>;

}
#else

#include <optional>
namespace result_type {
    template<typename T>
    using Optional = std::optional<T>;
}

#endif


#endif //CPPRESULTTYPE_OPTIONAL_HPP
