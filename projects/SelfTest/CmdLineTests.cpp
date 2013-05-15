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


    template<typename ConfigT>
    class CommandLine {
        struct ArgToken {
            enum TokenType {
                Positional,
                ShortOpt,
                LongOpt
            };
            ArgToken() : type( Positional ) {}
            ArgToken( TokenType _type, std::string const& _arg )
                : type( _type ), arg( _arg ) {}

            TokenType type;
            std::string arg;
        };

        class ArgBinder {
        public:
            ArgBinder( CommandLine* cl ) : m_cl( cl ) {}

            template<typename F>
            ArgBinder& bind( F f ) {
                if( !m_cl->opts.empty() )
                    m_cl->opts.back().validate();
                m_cl->opts.push_back( Opt( makeBoundField( f ) ) );
                return *this;
            }

            ArgBinder& shortOpt( std::string const& name ) {
                m_cl->opts.back().shortNames.push_back( name );
                return *this;
            }
            ArgBinder& longOpt( std::string const& name ) {
                m_cl->opts.back().longName = name;
                return *this;
            }
            ArgBinder& describe( std::string const& description ) {
                m_cl->opts.back().description = description;
                return *this;
            }
            ArgBinder& argName( std::string const& argName ) {
                m_cl->opts.back().argName = argName;
                return *this;
            }
            ArgBinder& position( int /*position*/ ) {
                // !TBD: Support for positional args in fixed positions
                return *this;
            }
        private:
            CommandLine* m_cl;
        };

        struct Opt {
            public:
                Opt( BoundField<ConfigT> const& _boundField ) : boundField( _boundField ) {}

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
            
                BoundField<ConfigT> boundField;
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

        void parseInto( int argc, char const* argv[], ConfigT& config ) const {
            if( opts.empty() )
                throw std::logic_error( "No options or arguments specified" );
            opts.back().validate();
            
            std::vector<ArgToken> tokens;
            parseIntoTokens( argc, argv, tokens );            
            setFromTokens( tokens, config );
        }

        void usage( std::ostream& os ) const {
            typename std::vector<Opt>::const_iterator itBegin = opts.begin(), itEnd = opts.end(), it;
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
        
    private:
        void parseIntoTokens( int argc, char const* argv[], std::vector<ArgToken>& tokens ) const {
            for( int i = 1; i < argc; ++i ) {
                std::string arg = argv[i];
                while( !arg.empty() ) {
                    ArgToken token( ArgToken::Positional, arg );
                    arg = "";
                    if( token.arg[0] == '-' ) {
                        if( token.arg.size() > 1 && token.arg[1] == '-' )
                            token = ArgToken( ArgToken::LongOpt, token.arg.substr( 2 ) );
                        else
                            token = ArgToken( ArgToken::ShortOpt, token.arg.substr( 1 ) );
                    }
                    if( token.type != ArgToken::Positional ) {
                        std::size_t pos = token.arg.find_first_of( " \t=:" );
                        if( pos != std::string::npos ) {
                            arg = token.arg.substr( pos+1 );
                            token.arg = token.arg.substr( 0, pos );
                        }
                    }
                    tokens.push_back( token );
                }
            }            
        }
        void setFromTokens( std::vector<ArgToken>const & tokens, ConfigT& config ) const {
            for( std::size_t i = 0; i < tokens.size(); ++i ) {
                ArgToken const& token = tokens[i];
                typename std::vector<Opt>::const_iterator it = opts.begin(), itEnd = opts.end();
                for(; it != itEnd; ++it ) {
                    Opt const& opt = *it;
                    
                    if( ( token.type == ArgToken::ShortOpt && opt.hasShortName( token.arg ) ) ||
                        ( token.type == ArgToken::LongOpt && opt.hasLongName( token.arg ) ) ) {
                        if( opt.takesArg() ) {
                            if( i == tokens.size()-1 || tokens[i+1].type != ArgToken::Positional )
                                throw std::domain_error( "Expected argument to option " + token.arg );
                            opt.boundField.set( config, tokens[++i].arg );
                        }
                        else {
                            opt.boundField.setFlag( config );
                        }
                        break;
                    }
                    else if( token.type == ArgToken::Positional && opt.isPositional() ) {
                        opt.boundField.set( config, token.arg );
                        break;
                    }
                }
                if( it == itEnd )
                    unhandledTokens.push_back( token );
            }
        }
        
        std::vector<Opt> opts;
        mutable std::vector<ArgToken> unhandledTokens; // !TBD
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

TEST_CASE( "cmdline", "" ) {

    TestOpt config;
    Clara::CommandLine<TestOpt> parser;
    parser.bind( &TestOpt::fileName )
        .describe( "specifies output file" )
        .shortOpt( "o" )
        .longOpt( "output" )
        .argName( "<filename>" );

    SECTION( "plain filename" ) {
        const char* argv[] = { "test", "-o filename.ext" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "plain filename with colon" ) {
        const char* argv[] = { "test", "-o:filename.ext" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "plain filename with =" ) {
        const char* argv[] = { "test", "-o=filename.ext" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
    }
    SECTION( "long opt" ) {
        const char* argv[] = { "test", "--output %stdout" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "%stdout" );
    }
    
    parser.bind( &TestOpt::number )
            .shortOpt( "n" )
            .argName( "<an integral value>" );
    
    SECTION( "a number" ) {
        const char* argv[] = { "test", "-n 42" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.number == 42 );
    }
    SECTION( "not a number" ) {
        const char* argv[] = { "test", "-n forty-two" };

        CHECK_THROWS( parser.parseInto( sizeof(argv)/sizeof(char*), argv, config ) );
        CHECK( config.number == 0 );
    }
    
    SECTION( "two parsers" ) {

        TestOpt config1;
        TestOpt2 config2;
        Clara::CommandLine<TestOpt2> parser2;

        parser2.bind( &TestOpt2::description )
            .describe( "description" )
            .shortOpt( "d" )
            .longOpt( "description" )
            .argName( "<some text>" );
        
        const char* argv[] = { "test", "-n 42", "-d some text" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config1 );
        CHECK( config1.number == 42 );

// !TBD
//        parser2.parseRemainingArgs( parser, config2 );
//        CHECK( config2.description == "some text" );
        
    }

    SECTION( "methods" ) {
        parser.bind( &TestOpt::setValidIndex )
            .describe( "An index, which is an integer between 0 and 10, inclusive" )
            .shortOpt( "i" )
            .argName( "<index>" );

        SECTION( "in range" ) {
            const char* argv[] = { "test", "-i 3" };

            parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
            REQUIRE( config.index == 3 );
        }
        SECTION( "out of range" ) {
            const char* argv[] = { "test", "-i 42" };

            REQUIRE_THROWS( parser.parseInto( sizeof(argv)/sizeof(char*), argv, config ) );
        }
    }
    
    SECTION( "flags" ) {
        parser.bind( &TestOpt::flag )
            .describe( "A flag" )
            .shortOpt( "f" );
        
        SECTION( "set" ) {
            const char* argv[] = { "test", "-f" };

            parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
            REQUIRE( config.flag );
        }
        SECTION( "not set" ) {
            const char* argv[] = { "test" };

            parser.parseInto( sizeof(argv)/sizeof(char*), argv, config );
            REQUIRE( config.flag == false );
        }
    }
}

struct Config {
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
    std::vector<std::string> warnings;
    
    void abortAfterFirst() { abortAfter = 1; }
    void abortAfterX( int x ) { abortAfter = x; }
    void addWarning( std::string const& _warning ) { warnings.push_back( _warning ); }
};


TEST_CASE( "growing new Catch cli" ) {
    Clara::CommandLine<Config> parser;

    parser.bind( &Config::showHelp )
        .describe( "display usage information" )
        .shortOpt( "?")
        .shortOpt( "h")
        .longOpt( "help" );

    parser.bind( &Config::listTests )
        .describe( "list all (or matching) test cases" )
        .shortOpt( "l")
        .longOpt( "list" );

    parser.bind( &Config::listTags )
        .describe( "list all (or matching) tags" )
        .shortOpt( "t")
        .longOpt( "tags" );

    parser.bind( &Config::showPassingTests )
        .describe( "show passing test output" )
        .shortOpt( "p")
        .longOpt( "passing" );

    parser.bind( &Config::breakIntoDebugger )
        .describe( "break into debugger on failure" )
        .shortOpt( "b")
        .longOpt( "break" );

    parser.bind( &Config::noThrow )
        .describe( "Skip exception tests" )
        .shortOpt( "e")
        .longOpt( "nothrow" );

    parser.bind( &Config::fileName )
        .describe( "output filename" )
        .shortOpt( "o")
        .longOpt( "out" )
        .argName( "file name" );

    parser.bind( &Config::suiteName )
        .describe( "suite name" )
        .shortOpt( "n")
        .longOpt( "name" )
        .argName( "name" );

    parser.bind( &Config::abortAfterFirst )
        .describe( "abort at first failure" )
        .shortOpt( "a")
        .longOpt( "abort" );

    parser.bind( &Config::abortAfterX )
        .describe( "abort after x failures" )
        .shortOpt( "x")
        .longOpt( "abortx" )
        .argName( "number of failures" );

    parser.bind( &Config::addWarning )
        .describe( "enables warnings" )
        .shortOpt( "w")
        .longOpt( "warn" )
        .argName( "warning name" );

    std::cout << parser << std::endl;
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

