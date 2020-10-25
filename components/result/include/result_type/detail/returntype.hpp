//
// Created by User on 10/25/2020.
//

#ifndef CPPRESULTTYPE_RETURNTYPE_HPP
#define CPPRESULTTYPE_RETURNTYPE_HPP
#include <type_traits>
#include <result_type/typetraits.hpp>
#include <result_type/optional.hpp>
#include <result_type/result.hpp>

namespace result_type {
    namespace detail{
        template<typename PipeInput, typename FunctionReturnType, typename = void>
        struct ReturnTypeImpl;

        template<typename PipeInput, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, FunctionReturnType, std::enable_if_t<
                is_optional_type<PipeInput>::value
                && is_result_type<FunctionReturnType>::value
                && !is_optional_type<typename FunctionReturnType::ResultSuccessType>::value, void>> {
            using type = Result<Optional<typename FunctionReturnType::ResultSuccessType>, typename FunctionReturnType::ResultErrorType>;
        };

        template<typename PipeInput, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, FunctionReturnType, std::enable_if_t<
                is_result_type<PipeInput>::value
                && std::is_void<FunctionReturnType>::value
                , void>>
        {
            using type = Result<NothingType, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, FunctionReturnType, std::enable_if_t<
                is_result_type<PipeInput>::value
                && !std::is_void<FunctionReturnType>::value
                && !is_result_type<FunctionReturnType>::value
                , void>>
        {
            using type = Result<FunctionReturnType, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename FunctionReturnType>
        using ReturnType_t = typename detail::ReturnTypeImpl<PipeInput, FunctionReturnType>::type;
    }



}

#endif //CPPRESULTTYPE_RETURNTYPE_HPP
