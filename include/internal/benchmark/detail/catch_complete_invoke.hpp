/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Invoke with a special case for void

#ifndef TWOBLUECUBES_CATCH_DETAIL_COMPLETE_INVOKE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DETAIL_COMPLETE_INVOKE_HPP_INCLUDED

#include "../../catch_enforce.h"
#include "../../catch_meta.hpp"

#include <type_traits>
#include <utility>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename T>
            struct CompleteType { using type = T; };
            template <>
            struct CompleteType<void> { struct type {}; };

            template <typename T>
            using CompleteType_t = typename CompleteType<T>::type;

            template <typename Result>
            struct CompleteInvoker {
                template <typename Fun, typename... Args>
                static Result invoke(Fun&& fun, Args&&... args) {
                    return std::forward<Fun>(fun)(std::forward<Args>(args)...);
                }
            };
            template <>
            struct CompleteInvoker<void> {
                template <typename Fun, typename... Args>
                static CompleteType_t<void> invoke(Fun&& fun, Args&&... args) {
                    std::forward<Fun>(fun)(std::forward<Args>(args)...);
                    return {};
                }
            };

            // invoke and not return void :(
            template <typename Fun, typename... Args>
            CompleteType_t<FunctionReturnType<Fun, Args...>> complete_invoke(Fun&& fun, Args&&... args) {
                return CompleteInvoker<FunctionReturnType<Fun, Args...>>::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);
            }

            const std::string benchmarkErrorMsg = "a benchmark failed to run successfully";
        } // namespace Detail

        template <typename Fun>
        Detail::CompleteType_t<FunctionReturnType<Fun>> user_code(Fun&& fun) {
            CATCH_TRY{
                return Detail::complete_invoke(std::forward<Fun>(fun));
            } CATCH_CATCH_ALL{
                getResultCapture().benchmarkFailed(translateActiveException());
                CATCH_RUNTIME_ERROR(Detail::benchmarkErrorMsg);
            }
        }
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_DETAIL_COMPLETE_INVOKE_HPP_INCLUDED
