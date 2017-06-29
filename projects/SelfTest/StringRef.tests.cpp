#include "../include/internal/catch_stringref.h"
#include "../include/internal/catch_string.h"

#include "catch.hpp"

#include <cstring>

namespace Catch {

    // Implementation of test accessors
    struct StringRefTestAccess {
        static auto isOwned( StringRef const& stringRef ) -> bool {
            return stringRef.isOwned();
        }
        static auto isSubstring( StringRef const& stringRef ) -> bool {
            return stringRef.isSubstring();
        }
        static auto data( StringRef const& stringRef ) -> char const* {
            return stringRef.data();
        }
    };

    auto isOwned( StringRef const& stringRef ) -> bool {
        return StringRefTestAccess::isOwned( stringRef );
    }
    auto isSubstring( StringRef const& stringRef ) -> bool {
        return StringRefTestAccess::isSubstring( stringRef );
    }
    auto data( StringRef const& stringRef ) -> char const* {
        return StringRefTestAccess::data( stringRef );
    }
} // namespace Catch2

namespace Catch {
    inline auto toString( Catch::StringRef const& stringRef ) -> std::string {
        return std::string( data( stringRef ), stringRef.size() );
    }
} // namespace Catch

TEST_CASE( "StringRef", "[Strings]" ) {
    
    using Catch::StringRef;
    using Catch::String;
    
    SECTION( "Empty string" ) {
        StringRef empty;
        REQUIRE( empty.empty() );
        REQUIRE( empty.size() == 0 );
        REQUIRE( std::strcmp( empty.c_str(), "" ) == 0 );
    }
    
    SECTION( "From string literal" ) {
        StringRef s = "hello";
        REQUIRE( s.empty() == false );
        REQUIRE( s.size() == 5 );
        REQUIRE( isSubstring( s ) == false );
        
        auto rawChars = data( s );
        REQUIRE( std::strcmp( rawChars, "hello" ) == 0 );
        
        SECTION( "c_str() does not cause copy" ) {
            REQUIRE( isOwned( s ) == false );
            
            REQUIRE( s.c_str() == rawChars );
            
            REQUIRE( isOwned( s ) == false );
        }
    }
    SECTION( "From sub-string" ) {
        StringRef original = StringRef( "original string" ).substr(0, 8);
        REQUIRE( original == "original" );
        REQUIRE( isSubstring( original ) );
        REQUIRE( isOwned( original ) == false );
        
        original.c_str(); // Forces it to take ownership
        
        REQUIRE( isSubstring( original ) == false );
        REQUIRE( isOwned( original ) );
        
    }
    
    
    SECTION( "Substrings" ) {
        StringRef s = "hello world!";
        StringRef ss = s.substr(0, 5);
        
        SECTION( "zero-based substring" ) {
            REQUIRE( ss.empty() == false );
            REQUIRE( ss.size() == 5 );
            REQUIRE( std::strcmp( ss.c_str(), "hello" ) == 0 );
            REQUIRE( ss == "hello" );
        }
        SECTION( "c_str() causes copy" ) {
            REQUIRE( isSubstring( ss ) );
            REQUIRE( isOwned( ss ) == false );
            
            auto rawChars = data( ss );
            REQUIRE( rawChars == data( s ) ); // same pointer value
            REQUIRE( ss.c_str() != rawChars );
            
            REQUIRE( isSubstring( ss ) == false );
            REQUIRE( isOwned( ss ) );
            
            REQUIRE( data( ss ) != data( s ) ); // different pointer value
        }
        
        SECTION( "non-zero-based substring") {
            ss = s.substr( 6, 6 );
            REQUIRE( ss.size() == 6 );
            REQUIRE( std::strcmp( ss.c_str(), "world!" ) == 0 );
        }
        
        SECTION( "Pointer values of full refs should match" ) {
            StringRef s2 = s;
            REQUIRE( s.c_str() == s2.c_str() );
        }
        
        SECTION( "Pointer values of substring refs should not match" ) {
            REQUIRE( s.c_str() != ss.c_str() );
        }
    }
    
    SECTION( "Comparisons" ) {
        REQUIRE( StringRef("hello") == StringRef("hello") );
        REQUIRE( StringRef("hello") != StringRef("cello") );
    }
    
    SECTION( "From string" ) {
        String str = "hot potato";
        auto originalPointer = str.c_str();
        
        SECTION( "Copied" ) {
            // After a String is "copied" to a StringRef
            // It has only copied the pointer and size
            // - it does not take ownership
            StringRef copied = str;
            REQUIRE( copied == "hot potato" );
            REQUIRE( str == "hot potato" );
            REQUIRE( isOwned( copied ) == false );
            REQUIRE( data( copied ) == originalPointer );
        }
        SECTION( "Moved" ) {
            // After a String is *moved* to a StringRef
            // The StringRef takes ownership of the underlying data
            // and the String is left in an empty state
            StringRef copied = std::move( str );
            REQUIRE( copied == "hot potato" );
            REQUIRE( isOwned( copied ) );
            REQUIRE( str.empty() );
            REQUIRE( data( copied ) == originalPointer );
        }
    }
}
