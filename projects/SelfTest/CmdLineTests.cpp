/*
 *  Copyright 2013 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include "catch_test_case_info.h"

#pragma clang diagnostic ignored "-Wpadded"

namespace Catch {
    class TestSpec {
        struct Pattern : SharedImpl<> {
            virtual ~Pattern();
            virtual bool matches( TestCaseInfo const& testCase ) const = 0;
        };
        class NamePattern : public Pattern {
            enum WildcardPosition {
                NoWildcard = 0,
                WildcardAtStart = 1,
                WildcardAtEnd = 2,
                WildcardAtBothEnds = WildcardAtStart | WildcardAtEnd
            };

        public:
            NamePattern( std::string const& name ) : m_name( toLower( name ) ), m_wildcard( NoWildcard ) {
                if( startsWith( m_name, "*" ) ) {
                    m_name = name.substr( 1 );
                    m_wildcard = WildcardAtStart;
                }
                if( endsWith( m_name, "*" ) ) {
                    m_name = m_name.substr( 0, m_name.size()-1 );
                    m_wildcard = (WildcardPosition)( m_wildcard | WildcardAtEnd );
                }
            }
            virtual ~NamePattern();
            virtual bool matches( TestCaseInfo const& testCase ) const {
                switch( m_wildcard ) {
                    case NoWildcard:
                        return m_name == toLower( testCase.name );
                    case WildcardAtStart:
                        return endsWith( toLower( testCase.name ), m_name );
                    case WildcardAtEnd:
                        return startsWith( toLower( testCase.name ), m_name );
                    case WildcardAtBothEnds:
                        return contains( toLower( testCase.name ), m_name );
                }
            }
        private:
            std::string m_name;
            WildcardPosition m_wildcard;
        };
        class TagPattern : public Pattern {
        public:
            TagPattern( std::string const& tag ) : m_tag( toLower( tag ) ) {}
            virtual ~TagPattern();
            virtual bool matches( TestCaseInfo const& testCase ) const {
                return testCase.tags.find( m_tag ) != testCase.tags.end();
            }
        private:
            std::string m_tag;
        };
        class ExcludedPattern : public Pattern {
        public:
            ExcludedPattern( Ptr<Pattern> const& underlyingPattern ) : m_underlyingPattern( underlyingPattern ) {}
            virtual ~ExcludedPattern();
            virtual bool matches( TestCaseInfo const& testCase ) const { return !m_underlyingPattern->matches( testCase ); }
        private:
            Ptr<Pattern> m_underlyingPattern;
        };

        struct Filter {
            std::vector<Ptr<Pattern> > m_patterns;

            bool matches( TestCaseInfo const& testCase ) const {
                // All patterns in a filter must match for the filter to be a match
                for( std::vector<Ptr<Pattern> >::const_iterator it = m_patterns.begin(), itEnd = m_patterns.end(); it != itEnd; ++it )
                    if( !(*it)->matches( testCase ) )
                        return false;
                    return true;
            }
        };

    public:
        bool hasFilters() const {
            return !m_filters.empty();
        }
        bool matches( TestCaseInfo const& testCase ) const {
            // A TestSpec matches if any filter matches
            for( std::vector<Filter>::const_iterator it = m_filters.begin(), itEnd = m_filters.end(); it != itEnd; ++it )
                if( it->matches( testCase ) )
                    return true;
            return false;
        }

    private:
        std::vector<Filter> m_filters;

        friend class TestSpecParser;
    };

    class TestSpecParser {
        enum Mode{ None, Name, QuotedName, Tag };
        Mode m_mode;
        bool m_exclusion;
        std::size_t m_start, m_pos;
        std::string m_arg;
        TestSpec::Filter m_currentFilter;

    public:
        TestSpec testSpec;

    public:
        TestSpecParser( std::string const& arg ) : m_mode( None ), m_exclusion( false ), m_start( std::string::npos ), m_arg( arg ) {
            for( m_pos = 0; m_pos < m_arg.size(); ++m_pos )
                visitChar( m_arg[m_pos] );
            visitChar( ',' );
        }
    private:
        void visitChar( char c ) {
            if( m_mode == None ) {
                switch( c ) {
                case ' ': return;
                case '~': m_exclusion = true; return;
                case '[': m_mode = Tag; m_start = ++m_pos; return;
                case '"': m_mode = QuotedName; m_start = m_pos+1; return;
                default: m_mode = Name; m_start = m_pos; break;
                }
            }
            if( m_mode == Name ) {
                if( c == ',' ) {
                    addPattern<TestSpec::NamePattern>();
                    addFilter();
                }
                else if( c == '[' ) {
                    if( subString() == "exclude:" )
                        m_exclusion = true;
                    else
                        addPattern<TestSpec::NamePattern>();
                    m_mode = Tag;
                    m_start = m_pos+1;
                }
            }
            else if( m_mode == QuotedName && c == '"' )
                addPattern<TestSpec::NamePattern>();
            else if( m_mode == Tag && c == ']' )
                addPattern<TestSpec::TagPattern>();
        }
        std::string subString() const { return m_arg.substr( m_start, m_pos - m_start ); }
        template<typename T>
        void addPattern() {
            std::string token = subString();
            if( startsWith( token, "exclude:" ) ) {
                m_exclusion = true;
                token = token.substr( 8 );
            }
            if( !token.empty() ) {
                Ptr<TestSpec::Pattern> pattern = new T( token );
                if( m_exclusion )
                    pattern = new TestSpec::ExcludedPattern( pattern );
                m_currentFilter.m_patterns.push_back( pattern );
            }
            m_exclusion = false;
            m_mode = None;
        }
        void addFilter() {
            if( !m_currentFilter.m_patterns.empty() ) {
                testSpec.m_filters.push_back( m_currentFilter );
                m_currentFilter = TestSpec::Filter();
            }
        }
    };
    inline TestSpec parseTestSpec( std::string const& arg ) {
        return TestSpecParser( arg ).testSpec;
    }

    // !TBD: move these to impl
    TestSpec::Pattern::~Pattern() {}
    TestSpec::NamePattern::~NamePattern() {}
    TestSpec::TagPattern::~TagPattern() {}
    TestSpec::ExcludedPattern::~ExcludedPattern() {}

} // namespace Catch

inline Catch::TestCase fakeTestCase( const char* name, const char* desc = "" ){ return Catch::makeTestCase( NULL, "", name, desc, CATCH_INTERNAL_LINEINFO ); }

TEST_CASE( "Parse test names and tags", "" ) {

    using Catch::parseTestSpec;
    using Catch::TestSpec;

    Catch::TestCase tcA = fakeTestCase( "a", "" );
    Catch::TestCase tcB = fakeTestCase( "b", "[one][x]" );
    Catch::TestCase tcC = fakeTestCase( "longer name with spaces", "[two][three][.][x]" );
    Catch::TestCase tcD = fakeTestCase( "zlonger name with spacesz", "" );

    SECTION( "Empty test spec should have no filters" ) {
        TestSpec spec;
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from empty string should have no filters" ) {
        TestSpec spec = parseTestSpec( "" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches(tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from just a comma should have no filters" ) {
        TestSpec spec = parseTestSpec( "," );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from name should have one filter" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
    }

    SECTION( "Test spec from quoted name should have one filter" ) {
        TestSpec spec = parseTestSpec( "\"b\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
    }

    SECTION( "Test spec from name should have one filter" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }

    SECTION( "Wildcard at the start" ) {
        TestSpec spec = parseTestSpec( "*spaces" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
        CHECK( parseTestSpec( "*a" ).matches( tcA ) == true );
    }
    SECTION( "Wildcard at the end" ) {
        TestSpec spec = parseTestSpec( "long*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
        CHECK( parseTestSpec( "a*" ).matches( tcA ) == true );
    }
    SECTION( "Wildcard at both ends" ) {
        TestSpec spec = parseTestSpec( "*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
        CHECK( parseTestSpec( "*a*" ).matches( tcA ) == true );
    }
    SECTION( "Just wildcard" ) {
        TestSpec spec = parseTestSpec( "*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }

    SECTION( "Single tag" ) {
        TestSpec spec = parseTestSpec( "[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }
    SECTION( "Single tag, two matches" ) {
        TestSpec spec = parseTestSpec( "[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Two tags" ) {
        TestSpec spec = parseTestSpec( "[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Wildcarded name and tag" ) {
        TestSpec spec = parseTestSpec( "*name*[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "Single tag exclusion" ) {
        TestSpec spec = parseTestSpec( "~[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "One tag exclusion and one tag inclusion" ) {
        TestSpec spec = parseTestSpec( "~[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }
    SECTION( "One tag exclusion and one wldcarded name inclusion" ) {
        TestSpec spec = parseTestSpec( "~[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "One tag exclusion, using exclude:, and one wldcarded name inclusion" ) {
        TestSpec spec = parseTestSpec( "exclude:[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "name exclusion" ) {
        TestSpec spec = parseTestSpec( "~b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "wildcarded name exclusion" ) {
        TestSpec spec = parseTestSpec( "~*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "wildcarded name exclusion with tag inclusion" ) {
        TestSpec spec = parseTestSpec( "~*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "wildcarded name exclusion, using exclude:, with tag inclusion" ) {
        TestSpec spec = parseTestSpec( "exclude:*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "two wildcarded names" ) {
        TestSpec spec = parseTestSpec( "\"longer*\"\"*spaces\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "empty tag" ) {
        TestSpec spec = parseTestSpec( "[]" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "empty quoted name" ) {
        TestSpec spec = parseTestSpec( "\"\"" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }

}
