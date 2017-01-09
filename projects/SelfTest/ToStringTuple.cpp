#include "catch.hpp"

#ifdef CATCH_CPP11_OR_GREATER

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

TEST_CASE( "tuple<>", "[toString][tuple][c++11][.]" )
{
    typedef std::tuple<> type;
    CHECK( "{ }" == Catch::toString(type{}) );
    type value {};
    CHECK( "{ }" == Catch::toString(value) );
}

TEST_CASE( "tuple<int>", "[toString][tuple][c++11][.]" )
{
    typedef std::tuple<int> type;
    CHECK( "{ 0 }" == Catch::toString(type{0}) );
}


TEST_CASE( "tuple<float,int>", "[toString][tuple][c++11][.]" )
{
    typedef std::tuple<float,int> type;
    CHECK( "1.2f" == Catch::toString(float(1.2)) );
    CHECK( "{ 1.2f, 0 }" == Catch::toString(type{1.2,0}) );
}

TEST_CASE( "tuple<string,string>", "[toString][tuple][c++11][.]" )
{
    typedef std::tuple<std::string,std::string> type;
    CHECK( "{ \"hello\", \"world\" }" == Catch::toString(type{"hello","world"}) );
}

TEST_CASE( "tuple<tuple<int>,tuple<>,float>", "[toString][tuple][c++11][.]" )
{
    typedef std::tuple<std::tuple<int>,std::tuple<>,float> type;
    type value { std::tuple<int>{42}, {}, 1.2f };
    CHECK( "{ { 42 }, { }, 1.2f }" == Catch::toString(value) );
}

#ifdef CATCH_CONFIG_CPP11_NULLPTR
TEST_CASE( "tuple<nullptr,int,const char *>", "[toString][tuple][c++11][.]" )
{
    typedef std::tuple<std::nullptr_t,int,const char *> type;
    type value { nullptr, 42, "Catch me" };
    CHECK( "{ nullptr, 42, \"Catch me\" }" == Catch::toString(value) );
}
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif /* #ifdef CATCH_CPP11_OR_GREATER */

