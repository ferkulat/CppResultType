//
// Created by marcel on 10/26/20.
//

#ifndef CPPRESULTTYPE_EXITCODE_HPP
#define CPPRESULTTYPE_EXITCODE_HPP
#include <result_type/typetraits.hpp>

namespace result_type::helper {
    template<typename T> struct intoExitCode_converts_Result_or_int_into_but_this_type_is;
    namespace detail{

        template<typename T, typename  = void>
        struct intoExitCodeImpl{
            constexpr static int get(T const& item) {
                intoExitCode_converts_Result_or_int_into_but_this_type_is<T>{};
                return -1;
            }
        };

        template<typename T>
        struct intoExitCodeImpl<T, std::enable_if_t< std::is_same<T, int>::value,void>>{
            constexpr static int get(T item){
                return item;
            }
        };
        template<typename T>
        struct intoExitCodeImpl<T, std::enable_if_t< result_type::is_result_type<T>::value,void>>{
            constexpr static int get(T const& item) {
                if (isError(item)) {
                    return -1;
                }
                return 0;
            }
        };
    }


    struct intoExitCode{
        template<typename T>
        auto operator()(T const& item)->int{
            return detail::intoExitCodeImpl<T>::get(item);
        }
    };


}

#endif //CPPRESULTTYPE_EXITCODE_HPP
