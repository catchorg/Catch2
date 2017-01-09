#include "catch.hpp"

/*
  TODO: maybe ought to check that user-provided specialisations of
 Catch::toString also do the right thing
*/

// Enum without user-provided stream operator
enum Enum1 { Enum1Value0, Enum1Value1 };

TEST_CASE( "toString(enum)", "[toString][enum]" ) {
    Enum1 e0 = Enum1Value0;
    CHECK( Catch::toString(e0) == "0" );
    Enum1 e1 = Enum1Value1;
    CHECK( Catch::toString(e1) == "1" );
}

// Enum with user-provided stream operator
enum Enum2 { Enum2Value0, Enum2Value1 };

inline std::ostream& operator<<( std::ostream& os, Enum2 v ) {
    return os << "E2{" << static_cast<int>(v) << "}";
}

TEST_CASE( "toString(enum w/operator<<)", "[toString][enum]" ) {
    Enum2 e0 = Enum2Value0;
    CHECK( Catch::toString(e0) == "E2{0}" );
    Enum2 e1 = Enum2Value1;
    CHECK( Catch::toString(e1) == "E2{1}" );
}

#if defined(CATCH_CPP11_OR_GREATER)
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

// Enum class without user-provided stream operator
enum class EnumClass1 { EnumClass1Value0, EnumClass1Value1 };

TEST_CASE( "toString(enum class)", "[toString][enum][enumClass][c++11][.]" ) {
    EnumClass1 e0 = EnumClass1::EnumClass1Value0;
    CHECK( Catch::toString(e0) == "0" );
    EnumClass1 e1 = EnumClass1::EnumClass1Value1;
    CHECK( Catch::toString(e1) == "1" );
}

// Enum class with user-provided stream operator
enum class EnumClass2 : short { EnumClass2Value0, EnumClass2Value1 };

inline std::ostream& operator<<( std::ostream& os, EnumClass2 e2 ) {
    switch( static_cast<int>( e2 ) ) {
        case static_cast<int>( EnumClass2::EnumClass2Value0 ):
            return os << "E2/V0";
        case static_cast<int>( EnumClass2::EnumClass2Value1 ):
            return os << "E2/V1";
        default:
            return os << "Unknown enum value " << static_cast<int>( e2 );
    }
}

TEST_CASE( "toString(enum class w/operator<<)", "[toString][enum][enumClass][c++11][.]" ) {
    EnumClass2 e0 = EnumClass2::EnumClass2Value0;
    CHECK( Catch::toString(e0) == "E2/V0" );
    EnumClass2 e1 = EnumClass2::EnumClass2Value1;
    CHECK( Catch::toString(e1) == "E2/V1" );

    EnumClass2 e3 = static_cast<EnumClass2>(10);
    CHECK( Catch::toString(e3) == "Unknown enum value 10" );
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif // CATCH_CPP11_OR_GREATER

