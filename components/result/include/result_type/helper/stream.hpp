//
// Created by marcel on 10/26/20.
//

#ifndef CPPRESULTTYPE_STREAM_HPP
#define CPPRESULTTYPE_STREAM_HPP
#include <result_type/typetraits.hpp>

namespace result_type::helper {
    namespace helper_detail{

        template<typename OStreamType, typename T>
        void streamSuccessHelper(OStreamType& os, T const& t);

        template<typename T, typename = void>
        struct streamSuccessToImpl{
            template<typename OstreamType>
            static void stream(OstreamType& ostreamType, T const& t){
                ostreamType << t;
            }
        };

        template<typename T>
        struct streamSuccessToImpl<T,std::enable_if_t<is_optional_type<T>::value, void>>{
            template<typename OstreamType>
            static void stream(OstreamType& ostreamType, T const& t){
                if(t.has_value()) {
                    streamSuccessHelper(ostreamType, t.value());
                }
            }
        };

        template<typename T>
        struct streamSuccessToImpl<T,std::enable_if_t<is_result_type<T>::value, void>>{
            template<typename OstreamType>
            static void stream(OstreamType& ostreamType, T const& t){
                if(isSuccess(t)){
                    streamSuccessHelper(ostreamType, t.CRefSuccess());
                }
            }
        };

        template<typename OStreamType, typename T>
        void streamSuccessHelper(OStreamType& os, T const& t){
            streamSuccessToImpl<T>::stream(os, t);
        }

        template <typename T, typename = void>
        struct streamToErrorImpl{
            template<typename OstreamType>
            static void stream(OstreamType&, T const&){

            }
        };

        template <typename T>
        struct streamToErrorImpl<T, std::enable_if_t<is_result_type<T>::value, void>>{
            template<typename OstreamType>
            static void stream(OstreamType& ostreamType, T const& t){
                if(isError(t)){
                    ostreamType << t.CRefError();
                }
            }
        };

        template<typename OStreamType, typename T>
        void streamErrorHelper(OStreamType& os, T const& t){
            streamToErrorImpl<T>::stream(os, t);
        }
    }

    template<typename OStreamType>
    auto streamSuccessTo(OStreamType& os){
        return [&os](auto&& item){
            helper_detail::streamSuccessHelper(os, item);
            return std::forward<decltype(item)>(item);
        };
    }

    template<typename OStreamType>
    auto streamErrorTo(OStreamType& os){
        return [&os](auto&& item){
            helper_detail::streamErrorHelper(os, item);
            return std::forward<decltype(item)>(item);
        };
    }

}
#endif //CPPRESULTTYPE_STREAM_HPP
