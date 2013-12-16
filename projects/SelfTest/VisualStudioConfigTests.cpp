/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#if defined(INTERNAL_CATCH_VS_MANAGED) || defined(INTERNAL_CATCH_VS_NATIVE)

namespace VisualStudioConfigTests
{
    struct Z1 {};
    TEST_CASE("VSRequire 1, initial config defaults are set", "[vs]")
    {
        CatchOverrides::ConfigReset<Z1> guard("VisualStudioConfigTests.cpp",0, 1);
        REQUIRE(!CatchOverrides::Config<Z1>::instance().includeSuccessfulResults("VisualStudioConfigTests.cpp",0));
        REQUIRE(!CatchOverrides::Config<Z1>::instance().warnAboutMissingAssertions("VisualStudioConfigTests.cpp",0));
        REQUIRE(CatchOverrides::Config<Z1>::instance().abortAfter("VisualStudioConfigTests.cpp",0) == 1);
        std::vector<std::string> tests = CatchOverrides::Config<Z1>::instance().listOfTests("VisualStudioConfigTests.cpp",0);
        REQUIRE(tests.empty());
    }

    struct Z2 {};
    TEST_CASE("VSRequire 2, initial config defaults are overridden", "[vs]")
    {
        CatchOverrides::ConfigShowSuccessfulTests<Z2> isr1("VisualStudioConfigTests.cpp",0, true);
        CatchOverrides::ConfigWarnMissingAssertions<Z2> wma1("VisualStudioConfigTests.cpp",0, true);
        CatchOverrides::ConfigAbortAfter<Z2> aa1("VisualStudioConfigTests.cpp",0, 42);
        CatchOverrides::ConfigAddTest<Z2> at1("VisualStudioConfigTests.cpp",0,"T1");
        CatchOverrides::ConfigReset<Z2> guard("VisualStudioConfigTests.cpp",0, 1);
        REQUIRE(CatchOverrides::Config<Z2>::instance().includeSuccessfulResults("VisualStudioConfigTests.cpp",0));
        REQUIRE(CatchOverrides::Config<Z2>::instance().warnAboutMissingAssertions("VisualStudioConfigTests.cpp",0));
        REQUIRE(CatchOverrides::Config<Z2>::instance().abortAfter("VisualStudioConfigTests.cpp",0) == 42);
        std::vector<std::string> tests = CatchOverrides::Config<Z2>::instance().listOfTests("VisualStudioConfigTests.cpp",0);
        REQUIRE(tests.size() == 1);
        REQUIRE(tests[0] == "T1");
    }

    struct Z3 {};
    TEST_CASE("VSRequire 3, initial config defaults are reset", "[vs]")
    {
        CatchOverrides::ConfigShowSuccessfulTests<Z3> isr1("VisualStudioConfigTests.cpp",0, true);
        CatchOverrides::ConfigWarnMissingAssertions<Z3> wma1("VisualStudioConfigTests.cpp",0, true);
        CatchOverrides::ConfigAbortAfter<Z3> aa1("VisualStudioConfigTests.cpp",0, 42);
        CatchOverrides::ConfigAddTest<Z3> at1("VisualStudioConfigTests.cpp",0,"T1");
        CatchOverrides::ConfigReset<Z3> guard0("VisualStudioConfigTests.cpp",0, 1);
        CatchOverrides::ConfigReset<Z3> guard1("VisualStudioConfigTests.cpp",1, 1);
        REQUIRE(CatchOverrides::Config<Z3>::instance().includeSuccessfulResults("VisualStudioConfigTests.cpp",0));
        REQUIRE(CatchOverrides::Config<Z3>::instance().warnAboutMissingAssertions("VisualStudioConfigTests.cpp",0));
        REQUIRE(CatchOverrides::Config<Z3>::instance().abortAfter("VisualStudioConfigTests.cpp",0) == 42);
        std::vector<std::string> tests = CatchOverrides::Config<Z3>::instance().listOfTests("VisualStudioConfigTests.cpp",0);
        REQUIRE(tests.size() == 1);
        REQUIRE(tests[0] == "T1");

        REQUIRE(!CatchOverrides::Config<Z3>::instance().includeSuccessfulResults("VisualStudioConfigTests.cpp",1));
        REQUIRE(!CatchOverrides::Config<Z3>::instance().warnAboutMissingAssertions("VisualStudioConfigTests.cpp",1));
        REQUIRE(CatchOverrides::Config<Z3>::instance().abortAfter("VisualStudioConfigTests.cpp",1) == 1);
        tests = CatchOverrides::Config<Z3>::instance().listOfTests("VisualStudioConfigTests.cpp",1);
        REQUIRE(tests.empty());
    }

    struct Z7 {};
    TEST_CASE("VSRequire 7, initial multi-line list config gets all values", "[vs]")
    {
        CatchOverrides::ConfigAddTest<Z7> a1("VisualStudioConfigTests.cpp",0,"T1");
        CatchOverrides::ConfigAddTest<Z7> a2("VisualStudioConfigTests.cpp",0,"T2");
        CatchOverrides::ConfigAddTest<Z7> a3("VisualStudioConfigTests.cpp",0,"T3");
        CatchOverrides::ConfigReset<Z7> guard0("VisualStudioConfigTests.cpp",0, 1);
        CatchOverrides::ConfigReset<Z7> guard1("VisualStudioConfigTests.cpp",1, 1);

        std::vector<std::string> tests = CatchOverrides::Config<Z7>::instance().listOfTests("VisualStudioConfigTests.cpp",0);
        REQUIRE(tests.size() == 3);
        REQUIRE(tests[0] == "T1");
        REQUIRE(tests[1] == "T2");
        REQUIRE(tests[2] == "T3");
        tests = CatchOverrides::Config<Z7>::instance().listOfTests("VisualStudioConfigTests.cpp",1);
        REQUIRE(tests.empty());
    }

    struct Z8 {};
    TEST_CASE("VSRequire 8, initial incrementing 'line' sets values", "[vs]")
    {
        CatchOverrides::ConfigShowSuccessfulTests<Z8> isr1("VisualStudioConfigTests.cpp",0, true);
        CatchOverrides::ConfigWarnMissingAssertions<Z8> wma1("VisualStudioConfigTests.cpp",1, true);
        CatchOverrides::ConfigAbortAfter<Z8> aa1("VisualStudioConfigTests.cpp",2, 42);
        CatchOverrides::ConfigAddTest<Z8> at1("VisualStudioConfigTests.cpp",3,"T1");
        CatchOverrides::ConfigReset<Z8> guard0("VisualStudioConfigTests.cpp",4, 1);
        CatchOverrides::ConfigReset<Z8> guard1("VisualStudioConfigTests.cpp",5, 1);
        REQUIRE(CatchOverrides::Config<Z8>::instance().includeSuccessfulResults("VisualStudioConfigTests.cpp",4));
        REQUIRE(CatchOverrides::Config<Z8>::instance().warnAboutMissingAssertions("VisualStudioConfigTests.cpp",4));
        REQUIRE(CatchOverrides::Config<Z8>::instance().abortAfter("VisualStudioConfigTests.cpp",4) == 42);
        std::vector<std::string> tests = CatchOverrides::Config<Z8>::instance().listOfTests("VisualStudioConfigTests.cpp",4);
        REQUIRE(tests.size() == 1);
        REQUIRE(tests[0] == "T1");

        REQUIRE(!CatchOverrides::Config<Z8>::instance().includeSuccessfulResults("VisualStudioConfigTests.cpp",5));
        REQUIRE(!CatchOverrides::Config<Z8>::instance().warnAboutMissingAssertions("VisualStudioConfigTests.cpp",5));
        REQUIRE(CatchOverrides::Config<Z8>::instance().abortAfter("VisualStudioConfigTests.cpp",5) == 1);
        tests = CatchOverrides::Config<Z8>::instance().listOfTests("VisualStudioConfigTests.cpp",5);
        REQUIRE(tests.empty());
    }

}
#endif
