//
// Created by User on 10/25/2020.
//

#ifndef CPPRESULTTYPE_RETURNTYPE_HPP
#define CPPRESULTTYPE_RETURNTYPE_HPP
#include <type_traits>
#include <result_type/typetraits.hpp>
#include <result_type/optional.hpp>
#include <result_type/result.hpp>
#include <result_type/detail/value_type_of.hpp>

namespace result_type {
    namespace detail{
        template<typename PipeInput, typename Function, typename FunctionReturnType, typename = void>
        struct ReturnTypeImpl;

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                is_optional_type<PipeInput>::value
                && is_result_type<FunctionReturnType>::value
                && !is_optional_type<typename FunctionReturnType::ResultSuccessType>::value, void>> {
            using type = Result<Optional<typename FunctionReturnType::ResultSuccessType>, typename FunctionReturnType::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                is_optional_type<PipeInput>::value
                && is_result_type<FunctionReturnType>::value
                && is_optional_type<typename FunctionReturnType::ResultSuccessType>::value, void>> {
            using type = FunctionReturnType;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithNonOptional<PipeInput>::value
                && std::is_void<FunctionReturnType>::value
                , void>>
        {
            using type = Result<NothingType, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithNonOptional<PipeInput>::value
                && !std::is_void<FunctionReturnType>::value
                && !is_result_type<FunctionReturnType>::value
                , void>>
        {
            using type = Result<FunctionReturnType, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithNonOptional<PipeInput>::value
                && is_result_type<FunctionReturnType>::value
                , void>>
        {
            using type = FunctionReturnType;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithOptional<PipeInput>::value
                && !isInvokeable<Function, typename PipeInput::ResultSuccessType>::value
                && std::is_void<FunctionReturnType>::value
                , void>>
        {
            using type = Result<Optional<NothingType>, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithOptional<PipeInput>::value
                && !isInvokeable<Function, typename PipeInput::ResultSuccessType>::value
                && !std::is_void<FunctionReturnType>::value
                , void>>
        {
            using type = Result<Optional<detail::value_type_of_t<FunctionReturnType>>, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithOptional<PipeInput>::value
                && isInvokeable<Function, typename PipeInput::ResultSuccessType>::value
                && !std::is_void<FunctionReturnType>::value
                && !is_result_type<FunctionReturnType>::value
                , void>>
        {
            using type = Result<FunctionReturnType, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithOptional<PipeInput>::value
                && isInvokeable<Function, typename PipeInput::ResultSuccessType>::value
                && isResultTypeWithNonOptional<FunctionReturnType>::value
                , void>>
        {
            using type = FunctionReturnType;
        };
        template<typename PipeInput, typename Function, typename FunctionReturnType>
        struct ReturnTypeImpl<PipeInput, Function, FunctionReturnType, std::enable_if_t<
                isResultTypeWithOptional<PipeInput>::value
                && isInvokeable<Function, typename PipeInput::ResultSuccessType>::value
                && std::is_void<FunctionReturnType>::value
                , void>>
        {
            using type = Result<NothingType, typename PipeInput::ResultErrorType>;
        };

        template<typename PipeInput, typename Function, typename FunctionReturnType>
        using ReturnType_t = typename detail::ReturnTypeImpl<typename std::remove_reference<PipeInput>::type , typename std::remove_reference<Function>::type, typename std::remove_reference<FunctionReturnType>::type>::type;
    }



}

#endif //CPPRESULTTYPE_RETURNTYPE_HPP
