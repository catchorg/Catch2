// X02-DisabledMacros.cpp
// Test that CATCH_CONFIG_DISABLE turns off TEST_CASE autoregistration
// and expressions in assertion macros are not run.

#include <catch2/catch_test_macros.hpp>

#include <iostream>

struct foo {
    foo(){
        REQUIRE_NOTHROW( print() );
    }
    void print() const {
        std::cout << "This should not happen\n";
    }
};

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif
// Construct foo, but `foo::print` should not be run
foo f;


#if defined(__clang__)
// The test is unused since the registration is disabled
#pragma clang diagnostic ignored "-Wunused-function"
#endif

// This test should not be run, because it won't be registered
TEST_CASE( "Disabled Macros" ) {
    std::cout << "This should not happen\n";
    FAIL();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
