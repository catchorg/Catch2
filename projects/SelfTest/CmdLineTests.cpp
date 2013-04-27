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
//#include "catch_ptr.hpp"

namespace Catch {

class ArgData {
public:
    ArgData( std::string const& _arg ) : m_weight( 2 )
    {
        std::size_t first = _arg.find( '<' );
        std::size_t last = _arg.find_last_of( '>' );
        if( first == std::string::npos || last == std::string::npos || last <= first+1 )
            throw std::logic_error( "Argument must contain a name in angle brackets but it was: " + _arg );
        m_prefix = _arg.substr( 0, first );
        m_postfix = _arg.substr( last+1 );
        m_name = _arg.substr( first+1, last-first-1 );
        if( !m_prefix.empty() )
            --m_weight;
        if( !m_postfix.empty() )
            --m_weight;
    }
    std::string const& name() const { return m_name; }
    std::string const& prefix() const { return m_prefix; }
    std::string const& postfix() const { return m_postfix; }

    bool isMatch( std::string const& _arg ) const {
        return startsWith( _arg, m_prefix ) && endsWith( _arg, m_postfix );
    }
    std::string strip( std::string const& _arg ) const {
        return _arg.substr( m_prefix.size(),
                            _arg.size() - m_prefix.size() - m_postfix.size() );
    }
    bool operator < ( ArgData const& _other ) const {
        return m_weight < _other.m_weight;
    }
protected:
    std::string m_prefix;
    std::string m_postfix;
    std::string m_name;
    int m_weight;
};

template<typename T>
class Opt {
public:
    Opt( std::string const& _synposis ) : m_synopsis( _synposis ) {}
    Opt& shortOpt( std::string const& _value ) { m_shortOpt = _value; return *this; }
    Opt& longOpt( std::string const& _value )  { m_longOpt = _value; return *this; }

    template<typename M>
    Opt& addArg( std::string const& _name, M const& _member  ){
        m_args.push_back( Arg( _name, _member ) );
        return *this;
    }

    std::string synopsis() const { return m_synopsis; }
    std::string shortOpt() const { return m_shortOpt; }
    std::string longOpt() const { return m_longOpt; }
    
    bool parseInto( std::string const& _arg, T& _config ) const {
        ensureWeightedArgsAreSorted();
        typename std::vector<Arg const*>::const_iterator
            it = m_argsInWeightedOrder.begin(),
            itEnd = m_argsInWeightedOrder.end();
        for( ; it != itEnd; ++it )
            if( (*it)->isMatch( _arg ) ) {
                (*it)->parseInto( _arg, _config );
                return true;
            }
        return false;
    }
    
private:

    struct IField : SharedImpl<> {
        virtual ~IField() {}
        virtual bool parseInto( std::string const& _arg, T& _config ) const = 0;
    };
    template<typename M>
    struct Field : IField {
        Field( M const& _member ) : member( _member ) {}
        bool parseInto( std::string const& _arg, T& _config ) const {
            std::stringstream ss;
            ss << _arg;
            ss >> _config.*member;
            return !ss.fail();
        }
        M member;
    };
    class Arg : public ArgData {
    public:
        Arg() : m_field( NULL ) {}
        template<typename M>
        Arg( std::string const& _name, M const& _member )
        :   ArgData( _name ),
            m_field( new Field<M>( _member ) )
        {}
        void parseInto( std::string const& _arg, T& _config ) const {
            if( !m_field->parseInto( strip( _arg ), _config ) )
                throw std::domain_error( "'" + _arg + "' was not valid for <" + m_name + ">" );
        }
        
    private:
        Ptr<IField> m_field;
    };
    
    static bool argLess( Arg const* lhs, Arg const* rhs ) {
        return *lhs < *rhs;
    }
    void ensureWeightedArgsAreSorted() const {
        if( m_args.size() > m_argsInWeightedOrder.size() ) {
            m_argsInWeightedOrder.clear();
            typename std::vector<Arg>::const_iterator   it = m_args.begin(),
                                                        itEnd = m_args.end();
            for( ; it != itEnd; ++it )
                m_argsInWeightedOrder.push_back( &*it );
            sort( m_argsInWeightedOrder.begin(), m_argsInWeightedOrder.end(), &Opt::argLess );
        }
    }
    std::string m_synopsis;
    std::string m_shortOpt;
    std::string m_longOpt;
    std::vector<Arg> m_args;
    mutable std::vector<Arg const*> m_argsInWeightedOrder;
};

template<typename T>
class CommandLineParser
{
public:
    void addOption( Opt<T> const& _opt ) {
        m_allOptionParsers.push_back( _opt );
        if( !_opt.shortOpt().empty() )
            m_optionsByName.insert( std::make_pair( "-" + _opt.shortOpt(), &m_allOptionParsers.back() ) );
        if( !_opt.longOpt().empty() )
            m_optionsByName.insert( std::make_pair( "--" + _opt.longOpt(), &m_allOptionParsers.back() ) );
    }
    bool parseArgs( int argc, const char* const argv[], T& config ) {
        for( int i = 0; i < argc; ++i ) {
            std::string fullArg = argv[i];
            if( fullArg[0] == '-' ) {
                std::string args, optName;
                std::size_t pos = fullArg.find( ':' );
                if( pos == std::string::npos ) {
                    optName = fullArg;
                }
                else {
                    optName = fullArg.substr(0, pos );
                    args = fullArg.substr( pos+1 );
                }
                typename std::map<std::string, Opt<T>*>::const_iterator it = m_optionsByName.find( optName );
                bool used = false;
                if( it != m_optionsByName.end() ) {
                    try {
                        used = it->second->parseInto( args, config );
                    }
                    catch( std::exception& ex ) {
                        throw std::domain_error( "Error in " + optName + " option: " + ex.what() );
                    }
                }
                if( !used )
                    m_unusedOpts.push_back( fullArg );
            }
            else {
                m_args.push_back( fullArg );
            }
        }
        return false;
    }

private:
    std::vector<Opt<T> > m_allOptionParsers;
    std::map<std::string, Opt<T>*> m_optionsByName;
    std::vector<std::string> m_args;
    std::vector<std::string> m_unusedOpts;
};

    
} // end namespace Catch

struct TestOpt {
    TestOpt() : number( 0 ) {}

    std::string fileName;
    std::string streamName;
    int number;
};

TEST_CASE( "Arg" ) {
    SECTION( "pre and post" ) {
        Catch::ArgData preAndPost( "prefix<arg>postfix" );
        CHECK( preAndPost.prefix() == "prefix" );
        CHECK( preAndPost.postfix() == "postfix" );
        CHECK( preAndPost.name() == "arg" );
        
        CHECK( preAndPost.isMatch( "prefixpayloadpostfix" ) );
        CHECK( preAndPost.strip( "prefixpayloadpostfix" ) == "payload" );
        CHECK_FALSE( preAndPost.isMatch( "payload" ) );
        CHECK_FALSE( preAndPost.isMatch( "postfixpayloadpostfix" ) );
        CHECK_FALSE( preAndPost.isMatch( "prefixpayloadpostfixx" ) );
    }
    SECTION( "pre" ) {
        Catch::ArgData preAndPost( "prefix<arg>" );
        CHECK( preAndPost.prefix() == "prefix" );
        CHECK( preAndPost.postfix() == "" );
        CHECK( preAndPost.name() == "arg" );
        
        CHECK( preAndPost.isMatch( "prefixpayload" ) );
        CHECK( preAndPost.strip( "prefixpayload" ) == "payload" );
        CHECK_FALSE( preAndPost.isMatch( "payload" ) );
        CHECK_FALSE( preAndPost.isMatch( "postfixpayload" ) );
    }
    SECTION( "post" ) {
        Catch::ArgData preAndPost( "<arg>postfix" );
        CHECK( preAndPost.prefix() == "" );
        CHECK( preAndPost.postfix() == "postfix" );
        CHECK( preAndPost.name() == "arg" );

        CHECK( preAndPost.isMatch( "payloadpostfix" ) );
        CHECK( preAndPost.strip( "payloadpostfix" ) == "payload" );
        CHECK_FALSE( preAndPost.isMatch( "payload" ) );
        CHECK_FALSE( preAndPost.isMatch( "payloadpostfixx" ) );
    }
    SECTION( "none" ) {
        Catch::ArgData preAndPost( "<arg>" );
        CHECK( preAndPost.prefix() == "" );
        CHECK( preAndPost.postfix() == "" );
        CHECK( preAndPost.name() == "arg" );

        CHECK( preAndPost.isMatch( "payload" ) );
        CHECK( preAndPost.strip( "payload" ) == "payload" );
    }
    SECTION( "errors" ) {
        CHECK_THROWS( Catch::ArgData( "" ) );
        CHECK_THROWS( Catch::ArgData( "no brackets" ) );
        CHECK_THROWS( Catch::ArgData( "<one bracket" ) );
        CHECK_THROWS( Catch::ArgData( "one bracket>" ) );
        CHECK_THROWS( Catch::ArgData( "><" ) );
        CHECK_THROWS( Catch::ArgData( "<>" ) );
    }
}

TEST_CASE( "cmdline", "" ) {

    Catch::Opt<TestOpt> opt( "specifies output file" );
    opt.shortOpt( "o" )
        .longOpt( "output" )
        .addArg( "<filename>", &TestOpt::fileName )
        .addArg( "%<stream name>", &TestOpt::streamName );

    TestOpt config;
    Catch::CommandLineParser<TestOpt> parser;
    parser.addOption( opt );

//    parser.addOption( "specifies output file" )
//        .shortOpt( "o" )
//        .longOpt( "output" )
//        .addArg( "<filename>", &TestOpt::fileName )
//        .addArg( "%<stream name>", &TestOpt::streamName );

    SECTION( "plain filename" ) {
        const char* argv[] = { "test", "-o:filename.ext" };

        parser.parseArgs( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "filename.ext" );
        CHECK( config.streamName == "" );
    }
    SECTION( "stream name" ) {
        const char* argv[] = { "test", "-o:%stdout" };

        parser.parseArgs( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "" );
        CHECK( config.streamName == "stdout" );
    }
    SECTION( "long opt" ) {
        const char* argv[] = { "test", "--output:%stdout" };

        parser.parseArgs( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.fileName == "" );
        CHECK( config.streamName == "stdout" );
    }
    
    Catch::Opt<TestOpt> opt2( "a number" );
    opt2.shortOpt( "n" )
        .addArg( "<an integral value>", &TestOpt::number );
    
    parser.addOption( opt2 );
    
    SECTION( "a number" ) {
        const char* argv[] = { "test", "-n:42" };

        parser.parseArgs( sizeof(argv)/sizeof(char*), argv, config );
        CHECK( config.number == 42 );
    }
    SECTION( "not a number" ) {
        const char* argv[] = { "test", "-n:forty-two" };

        CHECK_THROWS( parser.parseArgs( sizeof(argv)/sizeof(char*), argv, config ) );
        CHECK( config.number == 0 );
    }
    
}
