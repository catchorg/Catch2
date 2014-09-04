#include "catch.hpp"

#ifdef CATCH_CPP11_OR_GREATER

TEST_CASE( "tuple<>", "[toString][tuple]" )
{
    typedef std::tuple<> type;
    CHECK( "{ }" == Catch::toString(type{}) );
    type value {};
    CHECK( "{ }" == Catch::toString(value) );
}

TEST_CASE( "tuple<int>", "[toString][tuple]" )
{
    typedef std::tuple<int> type;
    CHECK( "{ 0 }" == Catch::toString(type{0}) );
}


TEST_CASE( "tuple<float,int>", "[toString][tuple]" )
{
    typedef std::tuple<float,int> type;
    CHECK( "1.2f" == Catch::toString(float(1.2)) );
    CHECK( "{ 1.2f, 0 }" == Catch::toString(type{1.2,0}) );
}

TEST_CASE( "tuple<string,string>", "[toString][tuple]" )
{
    typedef std::tuple<std::string,std::string> type;
    CHECK( "{ \"hello\", \"world\" }" == Catch::toString(type{"hello","world"}) );
}

TEST_CASE( "tuple<tuple<int>,tuple<>,float>", "[toString][tuple]" )
{
    typedef std::tuple<std::tuple<int>,std::tuple<>,float> type;
    type value { std::tuple<int>{42}, {}, 1.2f };
    CHECK( "{ { 42 }, { }, 1.2f }" == Catch::toString(value) );
}

TEST_CASE( "tuple<nullptr,int,const char *>", "[toString][tuple]" )
{
    typedef std::tuple<std::nullptr_t,int,const char *> type;
    type value { nullptr, 42, "Catch me" };
    CHECK( "{ nullptr, 42, \"Catch me\" }" == Catch::toString(value) );
}

TEST_CASE( "toString( tuple<pair,pair,pair> )", "[toString][tuple][pair]" )
{
    typedef std::tuple<std::pair<int,int>,
                       std::pair<int,int>,
                       std::pair<int,int>>
        type;
    std::pair<int,int> p1{ 12, 34 };
    CHECK( "{ 12, 34 }" == Catch::toString(p1) );
    std::pair<int,int> p2{ 23, 45 };
    CHECK( "{ 23, 45 }" == Catch::toString(p2) );
    std::pair<int,int> p3{ 24, 68 };
    CHECK( "{ 24, 68 }" == Catch::toString(p3) );
    type value{ p1, p2, p3 };
    CHECK( "{ { 12, 34 }, { 23, 45 }, { 24, 68 } }" == Catch::toString(value) );
}

TEST_CASE( "toString( pair<tuple,tuple> )", "[toString][tuple][pair]" )
{
    typedef std::pair<std::tuple<int,int,int>,
                      std::tuple<int,int,int>
                      > type;
    std::tuple<int,int,int> tuple1{ 12, 34, 56 };
    CHECK( "{ 12, 34, 56 }" == Catch::toString(tuple1) );
    std::tuple<int,int,int> tuple2{ 23, 45, 67 };
    CHECK( "{ 23, 45, 67 }" == Catch::toString(tuple2) );
    type value{ tuple1, tuple2 };
    CHECK( "{ { 12, 34, 56 }, { 23, 45, 67 } }" == Catch::toString(value) );
}

#endif /* #ifdef CATCH_CPP11_OR_GREATER */

