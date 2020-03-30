/*
 *  Created by Martin on 25/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/internal/catch_test_registry.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_test_case_registry_impl.hpp>
#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>

namespace Catch {

    std::unique_ptr<ITestInvoker> makeTestInvoker( void(*testAsFunction)() ) {
        return std::make_unique<TestInvokerAsFunction>( testAsFunction );
    }

    AutoReg::AutoReg( std::unique_ptr<ITestInvoker> invoker, SourceLineInfo const& lineInfo, StringRef const& classOrMethod, NameAndTags const& nameAndTags ) noexcept {
        CATCH_TRY {
            getMutableRegistryHub()
                    .registerTest(
                        makeTestCaseInfo(
                            extractClassName( classOrMethod ),
                            nameAndTags,
                            lineInfo),
                        std::move(invoker));
        } CATCH_CATCH_ALL {
            // Do not throw when constructing global objects, instead register the exception to be processed later
            getMutableRegistryHub().registerStartupException();
        }
    }
}
