/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch.hpp"
#include "catch_text.h"

namespace Clara {
    namespace Detail {
        template<typename T> struct RemoveConstRef{ typedef T type; };
        template<typename T> struct RemoveConstRef<T&>{ typedef T type; };
        template<typename T> struct RemoveConstRef<T const&>{ typedef T type; };
        template<typename T> struct RemoveConstRef<T const>{ typedef T type; };

        template<typename T>    struct IsBool       { static const bool value = false; };
        template<>              struct IsBool<bool> { static const bool value = true; };

        template<typename T>
        void convertInto( std::string const& _source, T& _dest ) {
            std::stringstream ss;
            ss << _source;
            ss >> _dest;
            if( ss.fail() )
                throw std::runtime_error( "Unable to convert " + _source + " to destination type" );
        }
        inline void convertInto( std::string const& _source, std::string& _dest ) {
            _dest = _source;
        }
        inline void convertInto( std::string const& _source, bool& _dest ) {
            std::string sourceLC = _source;
            std::transform( sourceLC.begin(), sourceLC.end(), sourceLC.begin(), ::tolower );
            if( sourceLC == "1" || sourceLC == "true" || sourceLC == "yes" || sourceLC == "on" )
                _dest = true;
            else if( sourceLC == "0" || sourceLC == "false" || sourceLC == "no" || sourceLC == "off" )
                _dest = false;
            else
                throw std::runtime_error( "Expected a boolean value but did recognise: '" + _source + "'" );
        }
        inline void convertInto( bool _source, bool& _dest ) {
            _dest = _source;
        }
        template<typename T>
        inline void convertInto( bool, T& ) {
            throw std::runtime_error( "Invalid conversion" );
        }

        template<typename ConfigT>
        struct IBoundMember {
            virtual ~IBoundMember() {}
            virtual void set( ConfigT& config, std::string const& value ) const = 0;
            virtual void setFlag( ConfigT& config ) const = 0;
            virtual bool takesArg() const = 0;
            virtual IBoundMember* clone() const = 0;
        };

        template<typename ConfigT>
        class BoundField {
        public:
            BoundField( IBoundMember<ConfigT>* _boundMember ) : boundMember( _boundMember ) {}
            BoundField( BoundField const& other ) : boundMember( other.boundMember->clone() ) {}
            BoundField& operator = ( BoundField const& other ) {
                IBoundMember<ConfigT> newMember = other.clone();
                delete boundMember;
                boundMember = newMember;
                return *this;
            }
            ~BoundField() { delete boundMember; }

            void set( ConfigT& config, std::string const& value ) const {
                boundMember->set( config, value );
            }
            void setFlag( ConfigT& config ) const {
                boundMember->setFlag( config );
            }
            bool takesArg() const { return boundMember->takesArg(); }
        private:
            IBoundMember<ConfigT>* boundMember;
        };


        template<typename C, typename M>
        struct BoundDataMember : IBoundMember<C>{
            BoundDataMember( M C::* _member ) : member( _member ) {}
            virtual void set( C& p, std::string const& stringValue ) const {
                convertInto( stringValue, p.*member );
            }
            virtual void setFlag( C& p ) const {
                convertInto( true, p.*member );
            }
            virtual bool takesArg() const { return !IsBool<M>::value; }
            virtual IBoundMember<C>* clone() const { return new BoundDataMember( *this ); }
            M C::* member;
        };
        template<typename C, typename M>
        struct BoundUnaryMethod : IBoundMember<C>{
            BoundUnaryMethod( void (C::*_member)( M ) ) : member( _member ) {}
            virtual void set( C& p, std::string const& stringValue ) const {
                typename RemoveConstRef<M>::type value;
                convertInto( stringValue, value );
                (p.*member)( value );
            }
            virtual void setFlag( C& p ) const {
                typename RemoveConstRef<M>::type value;
                convertInto( true, value );
                (p.*member)( value );
            }
            virtual bool takesArg() const { return !IsBool<M>::value; }
            virtual IBoundMember<C>* clone() const { return new BoundUnaryMethod( *this ); }
            void (C::*member)( M );
        };
        template<typename C>
        struct BoundNullaryMethod : IBoundMember<C>{
            BoundNullaryMethod( void (C::*_member)() ) : member( _member ) {}
            virtual void set( C& p, std::string const& stringValue ) const {
                bool value;
                convertInto( stringValue, value );
                if( value )
                    (p.*member)();
            }
            virtual void setFlag( C& p ) const {
                (p.*member)();
            }
            virtual bool takesArg() const { return false; }
            virtual IBoundMember<C>* clone() const { return new BoundNullaryMethod( *this ); }
            void (C::*member)();
        };
        
        template<typename C, typename M>
        BoundField<C> makeBoundField( M C::* _member ) {
            return BoundField<C>( new BoundDataMember<C,M>( _member ) );
        }
        template<typename C, typename M>
        BoundField<C> makeBoundField( void (C::*_member)( M ) ) {
            return BoundField<C>( new BoundUnaryMethod<C,M>( _member ) );
        }
        template<typename C>
        BoundField<C> makeBoundField( void (C::*_member)() ) {
            return BoundField<C>( new BoundNullaryMethod<C>( _member ) );
        }
    } // namespace Detail

    struct Parser {
        Parser() : separators( " \t=:" ) {}

        struct Token {
            enum Type { Positional, ShortOpt, LongOpt };
            Token( Type _type, std::string const& _data ) : type( _type ), data( _data ) {}
            Type type;
            std::string data;
        };

        void parseIntoTokens( int argc, char const* argv[], std::vector<Parser::Token>& tokens ) const {
            for( int i = 1; i < argc; ++i )
                parseIntoTokens( argv[i] , tokens);
        }        
        void parseIntoTokens( std::string arg, std::vector<Parser::Token>& tokens ) const {
            while( !arg.empty() ) {
                Parser::Token token( Parser::Token::Positional, arg );
                arg = "";
                if( token.data[0] == '-' ) {
                    if( token.data.size() > 1 && token.data[1] == '-' ) {
                        token = Parser::Token( Parser::Token::LongOpt, token.data.substr( 2 ) );
                    }
                    else {
                        token = Parser::Token( Parser::Token::ShortOpt, token.data.substr( 1 ) );
                        if( token.data.size() > 1 && separators.find( token.data[1] ) == std::string::npos ) {
                            arg = "-" + token.data.substr( 1 );
                            token.data = token.data.substr( 0, 1 );
                        }
                    }
                }
                if( token.type != Parser::Token::Positional ) {
                    std::size_t pos = token.data.find_first_of( separators );
                    if( pos != std::string::npos ) {
                        arg = token.data.substr( pos+1 );
                        token.data = token.data.substr( 0, pos );
                    }
                }
                tokens.push_back( token );
            }
        }
        std::string separators;
    };

    template<typename ConfigT>
    class CommandLine {

        class ArgBinder {
        public:
            ArgBinder( CommandLine* cl ) : m_cl( cl ) {}

            template<typename F>
            ArgBinder& bind( F f ) {
                if( !m_cl->m_args.empty() )
                    m_cl->m_args.back().validate();
                m_cl->m_args.push_back( Arg( Detail::makeBoundField( f ) ) );
                return *this;
            }
            ArgBinder& shortOpt( std::string const& name ) {
                m_cl->m_args.back().shortNames.push_back( name );
                return *this;
            }
            ArgBinder& longOpt( std::string const& name ) {
                m_cl->m_args.back().longName = name;
                return *this;
            }
            ArgBinder& describe( std::string const& description ) {
                m_cl->m_args.back().description = description;
                return *this;
            }
            ArgBinder& argName( std::string const& argName ) {
                m_cl->m_args.back().argName = argName;
                return *this;
            }
            ArgBinder& position( int /*position*/ ) {
                // !TBD: Support for positional args in fixed positions
                return *this;
            }
        private:
            CommandLine* m_cl;
        };

        struct Arg {
            Arg( Detail::BoundField<ConfigT> const& _boundField ) : boundField( _boundField ) {}

            bool hasShortName( std::string const& shortName ) const {
                for(    std::vector<std::string>::const_iterator
                            it = shortNames.begin(), itEnd = shortNames.end();
                        it != itEnd;
                        ++it )
                    if( *it == shortName )
                        return true;
                return false;
            }
            bool hasLongName( std::string const& _longName ) const {
                return _longName == longName;
            }
            bool takesArg() const {
                return !argName.empty();
            }
            bool isPositional() const {
                return shortNames.empty() && longName.empty();
            }
            std::string dbgName() const {
                if( !longName.empty() )
                    return "--" + longName;
                if( !shortNames.empty() )
                    return "-" + shortNames[0];
                return "positional args";
            }
            void validate() const {
                if( boundField.takesArg() && !takesArg() )
                    throw std::logic_error( dbgName() + " must specify an arg name" );
            }        
            std::string commands() const {
                std::ostringstream oss;
                bool first = true;
                std::vector<std::string>::const_iterator it = shortNames.begin(), itEnd = shortNames.end();
                for(; it != itEnd; ++it ) {
                    if( first )
                        first = false;
                    else
                        oss << ", ";
                    oss << "-" << *it;
                }
                if( !longName.empty() ) {
                    if( !first )
                        oss << ", ";
                    oss << "--" << longName;
                }
                if( !argName.empty() )
                    oss << " <" << argName << ">";
                return oss.str();
            }
        
            Detail::BoundField<ConfigT> boundField;
            std::vector<std::string> shortNames;
            std::string longName;
            std::string description;
            std::string argName;
        };

    public:
        template<typename F>
        ArgBinder bind( F f ) {
            ArgBinder binder( this );
            binder.bind( f );
            return binder;
        }

        void usage( std::ostream& os ) const {
            typename std::vector<Arg>::const_iterator itBegin = m_args.begin(), itEnd = m_args.end(), it;
            std::size_t maxWidth = 0;
            for( it = itBegin; it != itEnd; ++it )
                maxWidth = (std::max)( maxWidth, it->commands().size() );

            for( it = itBegin; it != itEnd; ++it ) {
                Catch::Text usage( it->commands(), Catch::TextAttributes().setWidth( maxWidth ) );
                // !TBD handle longer usage strings
                Catch::Text desc( it->description, Catch::TextAttributes().setWidth( CATCH_CONFIG_CONSOLE_WIDTH - maxWidth -3 ) );

                for( std::size_t i = 0; i < std::max( usage.size(), desc.size() ); ++i ) {
                    std::string usageCol = i < usage.size() ? usage[i] : "";
                    os << usageCol;

                    if( i < desc.size() && !desc[i].empty() )
                        os  << std::string( 2 + maxWidth - usageCol.size(), ' ' )
                            << desc[i];
                    os << "\n";
                }                
            }            
        }
        std::string usage() const {
            std::ostringstream oss;
            usage( oss );
            return oss.str();
        }
        friend std::ostream& operator << ( std::ostream& os, CommandLine const& parser ) {
            parser.usage( os );
            return os;
        }
        
        std::vector<Parser::Token> parseInto( int argc, char const* argv[], ConfigT& config ) const {
            std::vector<Parser::Token> tokens;
            Parser parser;
            parser.parseIntoTokens( argc, argv, tokens );
            return populate( tokens, config );
        }

        std::vector<Parser::Token> populate( std::vector<Parser::Token> const& tokens, ConfigT& config ) const {
            if( m_args.empty() )
                throw std::logic_error( "No options or arguments specified" );
            m_args.back().validate();

            std::vector<Parser::Token> unusedTokens;
            for( std::size_t i = 0; i < tokens.size(); ++i ) {
                Parser::Token const& token = tokens[i];
                typename std::vector<Arg>::const_iterator it = m_args.begin(), itEnd = m_args.end();
                for(; it != itEnd; ++it ) {
                    Arg const& arg = *it;
                    
                    if( ( token.type == Parser::Token::ShortOpt && arg.hasShortName( token.data ) ) ||
                        ( token.type == Parser::Token::LongOpt && arg.hasLongName( token.data ) ) ) {
                        if( arg.takesArg() ) {
                            if( i == tokens.size()-1 || tokens[i+1].type != Parser::Token::Positional )
                                throw std::domain_error( "Expected argument to option " + token.data );
                            arg.boundField.set( config, tokens[++i].data );
                        }
                        else {
                            arg.boundField.setFlag( config );
                        }
                        break;
                    }
                    else if( token.type == Parser::Token::Positional && arg.isPositional() ) {
                        arg.boundField.set( config, token.data );
                        break;
                    }
                }
                if( it == itEnd )
                    unusedTokens.push_back( token );
            }
            return unusedTokens;
        }
        
    private:
        std::vector<Arg> m_args;
    };
    
} // end namespace Clara

struct TestOpt {
    TestOpt() : number( 0 ), index( 0 ), flag( false ) {}

    std::string fileName;
    int number;
    int index;
    bool flag;
    
    void setValidIndex( int i ) {
        if( i < 0 || i > 10 )
            throw std::domain_error( "index must be between 0 and 10" );
        index = i;
    }
};

struct TestOpt2 {
    std::string description;
};

#ifdef CATCH_CONFIG_VARIADIC_MACROS

TEST_CASE( "cmdline" ) {

    TestOpt config;
    Clara::CommandLine<TestOpt> cli;
    cli.bind( &TestOpt::fileName )
        .describe( "specifies output file" )
        .shortOpt( "o" )
        .longOpt( "output" )
        .argName( "<filename>" );

    SECTION( "plain filename" ) {
        const char* argv[] = { "test", "-o filename.ext" };

        cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "plain filename with colon" ) {
        const char* argv[] = { "test", "-o:filename.ext" };

        cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "plain filename with =" ) {
        const char* argv[] = { "test", "-o=filename.ext" };

        cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "long opt" ) {
        const char* argv[] = { "test", "--output %stdout" };

        cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "%stdout" );
    }
    
    cli.bind( &TestOpt::number )
            .shortOpt( "n" )
            .argName( "<an integral value>" );
    
    SECTION( "a number" ) {
        const char* argv[] = { "test", "-n 42" };

        cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.number == 42 );
    }
    SECTION( "not a number" ) {
        const char* argv[] = { "test", "-n forty-two" };

        CHECK_THROWS( cli.parseInto( sizeof(argv)/sizeof(char*), argv, config ) );
        CHECK( config.number == 0 );
    }
    
    SECTION( "two parsers" ) {

        TestOpt config1;
        TestOpt2 config2;
        Clara::CommandLine<TestOpt2> cli2;

        cli2.bind( &TestOpt2::description )
            .describe( "description" )
            .shortOpt( "d" )
            .longOpt( "description" )
            .argName( "<some text>" );
        
        const char* argv[] = { "test", "-n 42", "-d some text" };

        std::vector<Clara::Parser::Token> unusedTokens = cli.parseInto( sizeof(argv)/sizeof(char*), argv, config1 );
        CHECK( config1.number == 42 );

        REQUIRE_FALSE( unusedTokens.empty() );
        cli2.populate( unusedTokens, config2 );
        CHECK( config2.description == "some text" );        
    }

    SECTION( "methods" ) {
        cli.bind( &TestOpt::setValidIndex )
            .describe( "An index, which is an integer between 0 and 10, inclusive" )
            .shortOpt( "i" )
            .argName( "<index>" );

        SECTION( "in range" ) {
            const char* argv[] = { "test", "-i 3" };

            cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
            REQUIRE( config.index == 3 );
        }
        SECTION( "out of range" ) {
            const char* argv[] = { "test", "-i 42" };

            REQUIRE_THROWS( cli.parseInto( sizeof(argv)/sizeof(char*), argv, config ) );
        }
    }
    
    SECTION( "flags" ) {
        cli.bind( &TestOpt::flag )
            .describe( "A flag" )
            .shortOpt( "f" );
        
        SECTION( "set" ) {
            const char* argv[] = { "test", "-f" };

            cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
            REQUIRE( config.flag );
        }
        SECTION( "not set" ) {
            const char* argv[] = { "test" };

            cli.parseInto( sizeof(argv)/sizeof(char*), argv, config );
            REQUIRE( config.flag == false );
        }
    }
}

struct Config {
    Config()
    :   listTests( false ),
        listTags( false ),
        showPassingTests( false ),
        breakIntoDebugger( false ),
        noThrow( false ),
        showHelp( false ),
        abortAfter( 0 )
    {}

    bool listTests;
    bool listTags;
    bool showPassingTests;
    bool breakIntoDebugger;
    bool noThrow;
    bool showHelp;
    
    int abortAfter;
    std::string reporterName;
    std::string fileName;
    std::string suiteName;
    std::string reporter;
    std::vector<std::string> warnings;
    std::vector<std::string> testsOrTags;
    
    void abortAfterFirst() { abortAfter = 1; }
    void abortAfterX( int x ) { abortAfter = x; }
    void addWarning( std::string const& _warning ) { warnings.push_back( _warning ); }
    void addTestOrTags( std::string const& _testSpec ) { testsOrTags.push_back( _testSpec ); }    
};


TEST_CASE( "growing new Catch cli" ) {
    Clara::CommandLine<Config> cli;

    cli.bind( &Config::showHelp )
        .describe( "display usage information" )
        .shortOpt( "?")
        .shortOpt( "h")
        .longOpt( "help" );

    cli.bind( &Config::listTests )
        .describe( "list all (or matching) test cases" )
        .shortOpt( "l")
        .longOpt( "list" );

    cli.bind( &Config::listTags )
        .describe( "list all (or matching) tags" )
        .shortOpt( "t")
        .longOpt( "tags" );

    cli.bind( &Config::showPassingTests )
        .describe( "show passing test output" )
        .shortOpt( "p")
        .longOpt( "passing" );

    cli.bind( &Config::breakIntoDebugger )
        .describe( "break into debugger on failure" )
        .shortOpt( "b")
        .longOpt( "break" );

    cli.bind( &Config::noThrow )
        .describe( "Skip exception tests" )
        .shortOpt( "e")
        .longOpt( "nothrow" );

    cli.bind( &Config::fileName )
        .describe( "output filename" )
        .shortOpt( "o")
        .longOpt( "out" )
        .argName( "file name" );

    cli.bind( &Config::reporter )
        .describe( "e.g. console | xml | junit" )
        .shortOpt( "r")
        .longOpt( "reporter" )
        .argName( "reporter name[:filename]" );

    cli.bind( &Config::suiteName )
        .describe( "suite name" )
        .shortOpt( "n")
        .longOpt( "name" )
        .argName( "name" );

    cli.bind( &Config::abortAfterFirst )
        .describe( "abort at first failure" )
        .shortOpt( "a")
        .longOpt( "abort" );

    cli.bind( &Config::abortAfterX )
        .describe( "abort after x failures" )
        .shortOpt( "x")
        .longOpt( "abortx" )
        .argName( "number of failures" );

    cli.bind( &Config::addWarning )
        .describe( "enables warnings" )
        .shortOpt( "w")
        .longOpt( "warn" )
        .argName( "warning name" );

    cli.bind( &Config::addTestOrTags )
        .describe( "which test or tests to use" )
        .argName( "test name, pattern or tags" );

    std::cout << cli << std::endl;
    
    Config config;

    const char* argv[] = { "test", "-peb" };
    int argc = sizeof(argv)/sizeof(char*);
    
    cli.parseInto( argc, argv, config );
    
    CHECK( config.showPassingTests );
    CHECK( config.noThrow );
    CHECK( config.breakIntoDebugger );
    
//
//        REQUIRE_THROWS( cli.parseInto( sizeof(argv)/sizeof(char*), argv, config ) );
    
}

// !TBD still support this?
//    Clara::Parser<Config>
//        ( "-h, --help                         display usage information",           &Config::showHelp )
//        ( "-l, --list                         list all (or matching) test cases",   &Config::listTests )
//        ( "-t, --tags                         list all (or matching) tags",         &Config::listTags )
//        ( "-p, --passing                      show passing test output",            &Config::showPassingTests )
//        ( "-b, --break                        break into debugger on failure",      &Config::breakIntoDebugger )
//        ( "-e, --nothrow                      Skip exception tests",                &Config::noThrow )
//        ( "-o, --out <file name>              output filename",                     &Config::fileName )
//        ( "-n, --name <name>                  suite name",                          &Config::suiteName )
//        ( "-a, --abort                        abort at first failure",              &Config::abortAfterFirst )
//        ( "-x, --abortx <number of failures>  abort after x failures",              &Config::abortAfterX )
//        ( "-w, --warn <warning name>          enables warnings",                    &Config::addWarning );
//        .parseInto( argc, argv, config );

#endif

