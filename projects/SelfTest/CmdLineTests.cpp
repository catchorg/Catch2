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

using namespace Catch;

template<typename T>
bool convertInto( std::string const& _source, T& _dest ) {
    std::stringstream ss;
    ss << _source;
    ss >> _dest;
    return !ss.fail();
}
inline bool convertInto( std::string const& _source, std::string& _dest ) {
    _dest = _source;
    return true;
}
inline bool convertInto( std::string const& _source, bool _dest ) {
    std::string sourceLC = toLower( _source );
    if( sourceLC == "1" || sourceLC == "true" || sourceLC == "yes" || sourceLC == "on" )
        _dest = true;
    else if( sourceLC == "0" || sourceLC == "false" || sourceLC == "no" || sourceLC == "off" )
        _dest = false;
    else
        return false;
    return true;
}

template<typename T>
class Opt {
public:
    Opt( std::string const& _synposis ) : m_synopsis( _synposis ) {}
    Opt& shortName( std::string const& _value ) { m_shortName = _value; return *this; }
    Opt& longName( std::string const& _value )  { m_longName = _value; return *this; }

    template<typename M>
    Opt& optArg( std::string const& _name, M const& _member  ){
        m_argName = _name;
        m_field = new Field<M>( _member );
        return *this;
    }

    template<typename M>
    Opt& flag( M const& _member  ){
        m_field = new FlagField<M>( _member );
        return *this;
    }
    
    std::size_t takesArg() const { return !m_argName.empty(); }
    std::string synopsis() const { return m_synopsis; }
    std::string shortName() const { return m_shortName; }
    std::string longName() const { return m_longName; }
    
    bool parseInto( std::string const& _arg, T& _config ) const {
        if( m_argName.empty() )
            m_field->set( _config );
        else if( !m_field->parseInto( _arg, _config ) )
            throw std::domain_error( "'" + _arg + "' was not valid for <" + m_argName + ">" );                
        return true;
    }

    std::string usage() const {
        std::ostringstream oss;
        writeToStream( oss );
        return oss.str();
    }
    friend std::ostream& operator << ( std::ostream& os, Opt const& _opt ) {
        _opt.writeToStream( os );
        return os;
    }
    void writeToStream ( std::ostream& os ) const {
        if( !m_shortName.empty() )
            os << "-" << m_shortName;
            if( !m_longName.empty() )
                os << ", ";
        if( !m_longName.empty() )
            os << "--" << m_longName;
        if( takesArg() )
            os << " <" << m_argName << ">";
    }    
private:

    struct IField : SharedImpl<> {
        virtual ~IField() {}
        virtual bool parseInto( std::string const&, T& ) const {
            throw std::logic_error( "cannot pass argument to bool binder" );
        }
        virtual void set( T& ) const {
            throw std::logic_error( "field requires an argument" );
        }
    };
        
    template<typename F>
    struct Field;

    template<typename F>
    struct FlagField;

    // Data member : with argument
    template<typename C, typename M>
    struct Field<M C::*> : IField {
        Field( M C::* _member ) : member( _member ) {}
        bool parseInto( std::string const& _arg, T& _config ) const {
            return convertInto( _arg, _config.*member );
        }
        M C::* member;
    };
    // Data member : flag
    template<typename C, typename M>
    struct FlagField<M C::*> : IField {
        FlagField( M C::* _member ) : member( _member ) {}
        void set( T& _config ) const {
            _config.*member = true;
        }
        M C::* member;
    };
    
    // Unary method : with argument
    template<typename C, typename M>
    struct Field<void (C::*)( M )> : IField {
        Field( void (C::*_method)( M ) ) : method( _method ) {}
        bool parseInto( std::string const& _arg, T& _config ) const {
            typename RemoveConstRef<M>::type value;
            if( !convertInto( _arg, value ) )
                return false;
            ( _config.*method )( value );
            return true;
        }
        void (C::*method)( M );
    };

    // Unary method : flag
    template<typename C, typename M>
    struct FlagField<void (C::*)( M )> : IField {
        FlagField( void (C::*_method)( M ) ) : method( _method ) {}
        void set( T& _config ) const {
            ( _config.*method )( true );
        }
        void (C::*method)( M );
    };

    // Nullary method : flag
    template<typename C>
    struct FlagField<void (C::*)()> : IField {
        FlagField( void (C::*_method)() ) : method( _method ) {}
        void set( T& _config ) const {
            ( _config.*method )();
        }
        void (C::*method)();
    };
    
    std::string m_synopsis;
    std::string m_shortName;
    std::string m_longName;
    std::string m_argName;
    Ptr<IField> m_field;
};

template<typename T>
class Parser
{
public:
    Parser()
    :   m_separatorChars( "=: " ),
        m_allowSpaceSeparator( m_separatorChars.find( ' ' ) != std::string::npos )
    {}
    template<typename M>
    Parser( std::string const&, M ) // !TBD
    :   m_separatorChars( "=: " ),
        m_allowSpaceSeparator( m_separatorChars.find( ' ' ) != std::string::npos )
    {}
    template<typename M>
    Parser& operator()( std::string const&, M ) { return *this; } // !TBD

    Opt<T>& addOption( std::string const& _synposis ) {
        m_allOptionParsers.push_back( _synposis );
        return m_allOptionParsers.back();
    }

    void parseInto( int argc, const char* const argv[], T& _config ) {
        std::vector<std::string> args;
        args.reserve( static_cast<std::size_t>( argc ) );
        for( int i = 0; i < argc; ++i )
            args.push_back( argv[i] );
        
        parseInto( args, _config );
    }

    template<typename U>
    void parseRemainingArgs( Parser<U>& _parser, T& _config ) {
        parseInto( _parser.m_unusedOpts, _config );
    }

    void parseInto( std::vector<std::string> const& _args, T& _config ) {
        ensureOptions();
        for( std::size_t i = 0; i < _args.size(); ++i ) {
            std::string const& arg = _args[i];
            if( arg[0] == '-' ) {
                std::string optArgs, optName;
                std::size_t pos = arg.find_first_of( m_separatorChars );
                if( pos == std::string::npos ) {
                    optName = arg;
                }
                else {
                    optName = arg.substr(0, pos );
                    optArgs = arg.substr( pos+1 );
                }
                typename std::map<std::string, Opt<T> const*>::const_iterator it = m_optionsByName.find( optName );
                bool used = false;
                if( it != m_optionsByName.end() ) {
                    Opt<T> const& opt = *(it->second);                    
                    if( opt.takesArg() ) {
                        if( optArgs.empty() ) {
                            if( i < _args.size() && _args[i+1][0] != '-' )
                                optArgs = _args[++i];
                            else
                                throw std::domain_error( "Expected argument"); // !TBD better error
                        }
                    }
                    try {
                        used = opt.parseInto( optArgs, _config );
                    }
                    catch( std::exception& ex ) {
                        throw std::domain_error( "Error in " + optName + " option: " + ex.what() );
                    }
                }
                if( !used )
                    m_unusedOpts.push_back( arg );
            }
            else {
                m_args.push_back( arg );
            }
        }
    }
    
    friend std::ostream& operator <<( std::ostream& os, Parser const& _parser ) {
        typename std::vector<Opt<T> >::const_iterator it, itEnd = _parser.m_allOptionParsers.end();
        std::size_t maxWidth = 0;
        for(it = _parser.m_allOptionParsers.begin(); it != itEnd; ++it )
            maxWidth = (std::max)( it->usage().size(), maxWidth );
        
        for(it = _parser.m_allOptionParsers.begin(); it != itEnd; ++it ) {
            Text usage( it->usage(), TextAttributes().setWidth( maxWidth ) );
            // !TBD handle longer usage strings
            Text synopsis( it->synopsis(), TextAttributes().setWidth( CATCH_CONFIG_CONSOLE_WIDTH - maxWidth -3 ) );

            for( std::size_t i = 0; i < std::max( usage.size(), synopsis.size() ); ++i ) {
                std::string usageCol = i < usage.size() ? usage[i] : "";
                std::cout << usageCol;

                if( i < synopsis.size() && !synopsis[i].empty() )
                    std::cout   << std::string( 2 + maxWidth - usageCol.size(), ' ' )
                                << synopsis[i];
                std::cout << "\n";
            }            
        }
        return os;
    }
    
private:
    void ensureOptions() const {
        if( m_allOptionParsers.size() != m_optionsByName.size() ) {
            m_optionsByName.clear();
            typename std::vector<Opt<T> >::const_iterator it, itEnd = m_allOptionParsers.end();
            for( it = m_allOptionParsers.begin(); it != itEnd; ++it ) {
                if( !it->shortName().empty() )
                    m_optionsByName.insert( std::make_pair( "-" + it->shortName(), &*it ) );
                if( !it->longName().empty() )
                    m_optionsByName.insert( std::make_pair( "--" + it->longName(), &*it ) );                
            }
        }
    }
    template<typename U>
    friend class Parser;

    std::vector<Opt<T> > m_allOptionParsers;
    mutable std::map<std::string, Opt<T> const*> m_optionsByName;
    std::vector<std::string> m_args;
    std::vector<std::string> m_unusedOpts;
    std::string m_separatorChars;
    bool m_allowSpaceSeparator;
};

    
} // end namespace Catch

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

TEST_CASE( "cmdline", "" ) {

    TestOpt config;
    Clara::Parser<TestOpt> parser;
    parser.addOption( "specifies output file" )
        .shortName( "o" )
        .longName( "output" )
        .optArg( "<filename>", &TestOpt::fileName );

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
    
    parser.addOption( "a number" )
            .shortName( "n" )
            .optArg( "<an integral value>", &TestOpt::number );
    
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
        Clara::Parser<TestOpt2> parser2;

        parser2.addOption( "description" )
                    .shortName( "d" )
                    .longName( "description" )
                    .optArg( "<some text>", &TestOpt2::description );
        
        const char* argv[] = { "test", "-n 42", "-d some text" };

        parser.parseInto( sizeof(argv)/sizeof(char*), argv, config1 );
        CHECK( config1.number == 42 );
        
        parser2.parseRemainingArgs( parser, config2 );
        CHECK( config2.description == "some text" );
        
    }

    SECTION( "methods" ) {
        parser.addOption( "An index, which is an integer between 0 and 10, inclusive" )
                .shortName( "i" )
                .optArg( "<index>", &TestOpt::setValidIndex );

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
        parser.addOption( "A flag" )
                .shortName( "f" )
                .flag( &TestOpt::flag );
        
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
    Clara::Parser<Config> parser;

    Clara::Parser<Config>
        ( "-h, --help                         display usage information",           &Config::showHelp )
        ( "-l, --list                         list all (or matching) test cases",   &Config::listTests )
        ( "-t, --tags                         list all (or matching) tags",         &Config::listTags )
        ( "-p, --passing                      show passing test output",            &Config::showPassingTests )
        ( "-b, --break                        break into debugger on failure",      &Config::breakIntoDebugger )
        ( "-e, --nothrow                      Skip exception tests",                &Config::noThrow )
        ( "-o, --out <file name>              output filename",                     &Config::fileName )
        ( "-n, --name <name>                  suite name",                          &Config::suiteName )
        ( "-a, --abort                        abort at first failure",              &Config::abortAfterFirst )
        ( "-x, --abortx <number of failures>  abort after x failures",              &Config::abortAfterX )
        ( "-w, --warn <warning name>          enables warnings",                    &Config::addWarning );
//        .parseInto( argc, argv, config );

    parser.addOption( "display usage information" )
        .shortName( "?")
        .shortName( "h")
        .longName( "help" )
        .flag( &Config::showHelp );

    parser.addOption( "list all (or matching) test cases" )
        .shortName( "l")
        .longName( "list" )
        .flag( &Config::listTests );

    parser.addOption( "list all (or matching) tags" )
        .shortName( "t")
        .longName( "tags" )
        .flag( &Config::listTags );

    parser.addOption( "show passing test output" )
        .shortName( "p")
        .longName( "passing" )
        .flag( &Config::showPassingTests );

    parser.addOption( "break into debugger on failure" )
        .shortName( "b")
        .longName( "break" )
        .flag( &Config::breakIntoDebugger );

    parser.addOption( "Skip exception tests" )
        .shortName( "e")
        .longName( "nothrow" )
        .flag( &Config::noThrow );

    parser.addOption( "output filename" )
        .shortName( "o")
        .longName( "out" )
        .optArg( "<file name>", &Config::fileName );

    parser.addOption( "suite name" )
        .shortName( "n")
        .longName( "name" )
        .optArg( "<name>", &Config::suiteName );

    parser.addOption( "abort at first failure" )
        .shortName( "a")
        .longName( "abort" )
        .flag( &Config::abortAfterFirst );

    parser.addOption( "abort after x failures" )
        .shortName( "x")
        .longName( "abortx" )
        .optArg( "<number of failures>", &Config::abortAfterX );

    parser.addOption( "enables warnings" )
        .shortName( "w")
        .longName( "warn" )
        .optArg( "<warning name>", &Config::addWarning );

    std::cout << parser << std::endl;
}
