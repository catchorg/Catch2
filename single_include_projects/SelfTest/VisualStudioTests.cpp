/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#if defined(INTERNAL_CATCH_VS_MANAGED) || defined(INTERNAL_CATCH_VS_NATIVE)

namespace VisualStudioTests
{
    class UniqueTestsFixture {
    private:
        static int uniqueID;
    public:
        UniqueTestsFixture() { }
    protected:
        int getID() {
            return ++uniqueID;
        }
    };

    int UniqueTestsFixture::uniqueID = 0;
    TEST_CASE("M00", "[m_off][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(!show);
        int abortAfter = Catch::getCurrentContext().getConfig()->abortAfter();
        REQUIRE(abortAfter == 1);
    }

    CATCH_CONFIG_SHOW_SUCCESS(true)
    TEST_CASE("M01", "[m_on][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(show);
    }

    TEST_CASE("M02", "[m_off][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(!show);
    }
    
    TEST_CASE_METHOD(UniqueTestsFixture, "M10", "[m_off][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(!show);
        getID();
    }

    CATCH_CONFIG_WARN_MISSING_ASSERTIONS(true)
    CATCH_CONFIG_SHOW_SUCCESS(true)
    TEST_CASE_METHOD(UniqueTestsFixture, "M11", "[m_on][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(show);
        getID();
    }

    CATCH_CONFIG_WARN_MISSING_ASSERTIONS(true)
    CATCH_CONFIG_SHOW_SUCCESS(true)
    TEST_CASE_METHOD(UniqueTestsFixture, "M99", "[m_on][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(show);
        WARN("Warning message");
        getID();
    }

    TEST_CASE_METHOD(UniqueTestsFixture, "M12", "[m_off][vs]")
    {
        bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
        REQUIRE(!show);
        getID();
    }

    class ConfigTest
    {
    public:
        void run1()
        {
            bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
            REQUIRE(!show);
        }
        void run2()
        {
            bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
            REQUIRE(show);
        }
        void run3()
        {
            bool show = Catch::getCurrentContext().getConfig()->includeSuccessfulResults();
            REQUIRE(!show);
        }
    };
    METHOD_AS_TEST_CASE(ConfigTest::run1,"M20", "[m_off][vs]");

    CATCH_CONFIG_SHOW_SUCCESS(true)
    METHOD_AS_TEST_CASE(ConfigTest::run2,"M21", "[m_on][vs]");

    METHOD_AS_TEST_CASE(ConfigTest::run3,"M22", "[m_off][vs]");

    CATCH_MAP_CATEGORY_TO_TAG(vstestsCheckOutputOff, "[m_off][vs]");
    CATCH_CONFIG_SHOW_SUCCESS(true)
    CATCH_MAP_CATEGORY_TO_TAG(vstestsCheckOutputOn, "[m_on][vs]");
    CATCH_MAP_CATEGORY_TO_TAG(vstestsCheckOutputOff2, "[m_off][vs]");
}
#endif
