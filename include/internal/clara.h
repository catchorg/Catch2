/*
 *  Created by Phil on 25/05/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Only use header guard if we are not using an outer namespace
#ifndef CLICHE_CLARA_OUTER_NAMESPACE
# ifdef TWOBLUECUBES_CLARA_H_INCLUDED
#  ifndef TWOBLUECUBES_CLARA_H_ALREADY_INCLUDED
#   define TWOBLUECUBES_CLARA_H_ALREADY_INCLUDED
#  endif
# else
#  define TWOBLUECUBES_CLARA_H_INCLUDED
# endif
#endif
#ifndef TWOBLUECUBES_CLARA_H_ALREADY_INCLUDED

#ifndef CLICHE_CLARA_OUTER_NAMESPACE
#define CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE Clara
#include "tbc_text_format.h"
#undef CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE
#endif

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <memory>

// Use optional outer namespace
#ifdef CLICHE_CLARA_OUTER_NAMESPACE
namespace CLICHE_CLARA_OUTER_NAMESPACE {
#endif

namespace Clara {
    namespace Detail {

#ifdef CLARA_CONSOLE_WIDTH
    const unsigned int consoleWidth = CLARA_CONFIG_CONSOLE_WIDTH;
#else
    const unsigned int consoleWidth = 80;
#endif

        using namespace Tbc;

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
            if( sourceLC == "y" || sourceLC == "1" || sourceLC == "true" || sourceLC == "yes" || sourceLC == "on" )
                _dest = true;
            else if( sourceLC == "n" || sourceLC == "0" || sourceLC == "false" || sourceLC == "no" || sourceLC == "off" )
                _dest = false;
            else
                throw std::runtime_error( "Expected a boolean value but did not recognise:\n  '" + _source + "'" );
        }
        inline void convertInto( bool _source, bool& _dest ) {
            _dest = _source;
        }
        template<typename T>
        inline void convertInto( bool, T& ) {
            throw std::runtime_error( "Invalid conversion" );
        }

        template<typename ConfigT>
        struct IArgFunction {
            virtual ~IArgFunction() {}
            virtual void set( ConfigT& config, std::string const& value ) const = 0;
            virtual void setFlag( ConfigT& config ) const = 0;
            virtual bool takesArg() const = 0;
            virtual IArgFunction* clone() const = 0;
        };

        template<typename ConfigT>
        class BoundArgFunction {
        public:
            BoundArgFunction( IArgFunction<ConfigT>* _functionObj ) : functionObj( _functionObj ) {}
            BoundArgFunction( BoundArgFunction const& other ) : functionObj( other.functionObj->clone() ) {}
            BoundArgFunction& operator = ( BoundArgFunction const& other ) {
                IArgFunction<ConfigT>* newFunctionObj = other.functionObj->clone();
                delete functionObj;
                functionObj = newFunctionObj;
                return *this;
            }
            ~BoundArgFunction() { delete functionObj; }

            void set( ConfigT& config, std::string const& value ) const {
                functionObj->set( config, value );
            }
            void setFlag( ConfigT& config ) const {
                functionObj->setFlag( config );
            }
            bool takesArg() const { return functionObj->takesArg(); }
        private:
            IArgFunction<ConfigT>* functionObj;
        };


        template<typename C>
        struct NullBinder : IArgFunction<C>{
            virtual void set( C&, std::string const& ) const {}
            virtual void setFlag( C& ) const {}
            virtual bool takesArg() const { return true; }
            virtual IArgFunction<C>* clone() const { return new NullBinder( *this ); }
        };

        template<typename C, typename M>
        struct BoundDataMember : IArgFunction<C>{
            BoundDataMember( M C::* _member ) : member( _member ) {}
            virtual void set( C& p, std::string const& stringValue ) const {
                convertInto( stringValue, p.*member );
            }
            virtual void setFlag( C& p ) const {
                convertInto( true, p.*member );
            }
            virtual bool takesArg() const { return !IsBool<M>::value; }
            virtual IArgFunction<C>* clone() const { return new BoundDataMember( *this ); }
            M C::* member;
        };
        template<typename C, typename M>
        struct BoundUnaryMethod : IArgFunction<C>{
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
            virtual IArgFunction<C>* clone() const { return new BoundUnaryMethod( *this ); }
            void (C::*member)( M );
        };
        template<typename C>
        struct BoundNullaryMethod : IArgFunction<C>{
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
            virtual IArgFunction<C>* clone() const { return new BoundNullaryMethod( *this ); }
            void (C::*member)();
        };

        template<typename C>
        struct BoundUnaryFunction : IArgFunction<C>{
            BoundUnaryFunction( void (*_function)( C& ) ) : function( _function ) {}
            virtual void set( C& obj, std::string const& stringValue ) const {
                bool value;
                convertInto( stringValue, value );
                if( value )
                    function( obj );
            }
            virtual void setFlag( C& p ) const {
                function( p );
            }
            virtual bool takesArg() const { return false; }
            virtual IArgFunction<C>* clone() const { return new BoundUnaryFunction( *this ); }
            void (*function)( C& );
        };

        template<typename C, typename T>
        struct BoundBinaryFunction : IArgFunction<C>{
            BoundBinaryFunction( void (*_function)( C&, T ) ) : function( _function ) {}
            virtual void set( C& obj, std::string const& stringValue ) const {
                typename RemoveConstRef<T>::type value;
                convertInto( stringValue, value );
                function( obj, value );
            }
            virtual void setFlag( C& obj ) const {
                typename RemoveConstRef<T>::type value;
                convertInto( true, value );
                function( obj, value );
            }
            virtual bool takesArg() const { return !IsBool<T>::value; }
            virtual IArgFunction<C>* clone() const { return new BoundBinaryFunction( *this ); }
            void (*function)( C&, T );
        };

        template<typename C, typename M>
        BoundArgFunction<C> makeBoundField( M C::* _member ) {
            return BoundArgFunction<C>( new BoundDataMember<C,M>( _member ) );
        }
        template<typename C, typename M>
        BoundArgFunction<C> makeBoundField( void (C::*_member)( M ) ) {
            return BoundArgFunction<C>( new BoundUnaryMethod<C,M>( _member ) );
        }
        template<typename C>
        BoundArgFunction<C> makeBoundField( void (C::*_member)() ) {
            return BoundArgFunction<C>( new BoundNullaryMethod<C>( _member ) );
        }
        template<typename C>
        BoundArgFunction<C> makeBoundField( void (*_function)( C& ) ) {
            return BoundArgFunction<C>( new BoundUnaryFunction<C>( _function ) );
        }
        template<typename C, typename T>
        BoundArgFunction<C> makeBoundField( void (*_function)( C&, T ) ) {
            return BoundArgFunction<C>( new BoundBinaryFunction<C, T>( _function ) );
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

        void parseIntoTokens( int argc, char const * const * argv, std::vector<Parser::Token>& tokens ) const {
            const std::string doubleDash = "--";
            for( int i = 1; i < argc && argv[i] != doubleDash; ++i )
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

        struct Arg {
            Arg( Detail::BoundArgFunction<ConfigT> const& _boundField ) : boundField( _boundField ), position( -1 ) {}

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
                return !hint.empty();
            }
            bool isFixedPositional() const {
                return position != -1;
            }
            bool isAnyPositional() const {
                return position == -1 && shortNames.empty() && longName.empty();
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
                if( !hint.empty() )
                    oss << " <" << hint << ">";
                return oss.str();
            }

            Detail::BoundArgFunction<ConfigT> boundField;
            std::vector<std::string> shortNames;
            std::string longName;
            std::string description;
            std::string hint;
            int position;
        };

        // NOTE: std::auto_ptr is deprecated in c++11/c++0x
#if defined(__cplusplus) && __cplusplus > 199711L
        typedef std::unique_ptr<Arg> ArgAutoPtr;
#else
        typedef std::auto_ptr<Arg> ArgAutoPtr;
#endif

        class ArgBinder {
        public:
            template<typename F>
            ArgBinder( CommandLine* cl, F f )
            :   m_cl( cl ),
                m_arg( Detail::makeBoundField( f ) )
            {}
            ArgBinder( ArgBinder& other )
            :   m_cl( other.m_cl ),
                m_arg( other.m_arg )
            {
                other.m_cl = NULL;
            }
            ~ArgBinder() {
                if( m_cl ) {
                    m_arg.validate();
                    if( m_arg.isFixedPositional() ) {
                        m_cl->m_positionalArgs.insert( std::make_pair( m_arg.position, m_arg ) );
                        if( m_arg.position > m_cl->m_highestSpecifiedArgPosition )
                            m_cl->m_highestSpecifiedArgPosition = m_arg.position;
                    }
                    else if( m_arg.isAnyPositional() ) {
                        if( m_cl->m_arg.get() )
                            throw std::logic_error( "Only one unpositional argument can be added" );
                        m_cl->m_arg = ArgAutoPtr( new Arg( m_arg ) );
                    }
                    else
                        m_cl->m_options.push_back( m_arg );
                }
            }
            ArgBinder& shortOpt( std::string const& name ) {
                m_arg.shortNames.push_back( name );
                return *this;
            }
            ArgBinder& longOpt( std::string const& name ) {
                m_arg.longName = name;
                return *this;
            }
            ArgBinder& describe( std::string const& description ) {
                m_arg.description = description;
                return *this;
            }
            ArgBinder& hint( std::string const& hint ) {
                m_arg.hint = hint;
                return *this;
            }
            ArgBinder& position( int position ) {
                m_arg.position = position;
                return *this;
            }
        private:
            CommandLine* m_cl;
            Arg m_arg;
        };

    public:

        CommandLine()
        :   m_boundProcessName( new Detail::NullBinder<ConfigT>() ),
            m_highestSpecifiedArgPosition( 0 ),
            m_throwOnUnrecognisedTokens( false )
        {}
        CommandLine( CommandLine const& other )
        :   m_boundProcessName( other.m_boundProcessName ),
            m_options ( other.m_options ),
            m_positionalArgs( other.m_positionalArgs ),
            m_highestSpecifiedArgPosition( other.m_highestSpecifiedArgPosition ),
            m_throwOnUnrecognisedTokens( other.m_throwOnUnrecognisedTokens )
        {
            if( other.m_arg.get() )
                m_arg = ArgAutoPtr( new Arg( *other.m_arg ) );
        }

        CommandLine& setThrowOnUnrecognisedTokens( bool shouldThrow = true ) {
            m_throwOnUnrecognisedTokens = shouldThrow;
            return *this;
        }

        template<typename F>
        ArgBinder bind( F f ) {
            ArgBinder binder( this, f );
            return binder;
        }
        template<typename F>
        void bindProcessName( F f ) {
            m_boundProcessName = Detail::makeBoundField( f );
        }

        void optUsage( std::ostream& os, std::size_t indent = 0, std::size_t width = Detail::consoleWidth ) const {
            typename std::vector<Arg>::const_iterator itBegin = m_options.begin(), itEnd = m_options.end(), it;
            std::size_t maxWidth = 0;
            for( it = itBegin; it != itEnd; ++it )
                maxWidth = (std::max)( maxWidth, it->commands().size() );

            for( it = itBegin; it != itEnd; ++it ) {
                Detail::Text usage( it->commands(), Detail::TextAttributes()
                                                        .setWidth( maxWidth+indent )
                                                        .setIndent( indent ) );
                // !TBD handle longer usage strings
                Detail::Text desc( it->description, Detail::TextAttributes()
                                                        .setWidth( width - maxWidth -3 ) );

                for( std::size_t i = 0; i < (std::max)( usage.size(), desc.size() ); ++i ) {
                    std::string usageCol = i < usage.size() ? usage[i] : "";
                    os << usageCol;

                    if( i < desc.size() && !desc[i].empty() )
                        os  << std::string( indent + 2 + maxWidth - usageCol.size(), ' ' )
                            << desc[i];
                    os << "\n";
                }
            }
        }
        std::string optUsage() const {
            std::ostringstream oss;
            optUsage( oss );
            return oss.str();
        }

        void argSynopsis( std::ostream& os ) const {
            for( int i = 1; i <= m_highestSpecifiedArgPosition; ++i ) {
                if( i > 1 )
                    os << " ";
                typename std::map<int, Arg>::const_iterator it = m_positionalArgs.find( i );
                if( it != m_positionalArgs.end() )
                    os << "<" << it->second.hint << ">";
                else if( m_arg.get() )
                    os << "<" << m_arg->hint << ">";
                else
                    throw std::logic_error( "non consecutive positional arguments with no floating args" );
            }
            // !TBD No indication of mandatory args
            if( m_arg.get() ) {
                if( m_highestSpecifiedArgPosition > 1 )
                    os << " ";
                os << "[<" << m_arg->hint << "> ...]";
            }
        }
        std::string argSynopsis() const {
            std::ostringstream oss;
            argSynopsis( oss );
            return oss.str();
        }

        void usage( std::ostream& os, std::string const& procName ) const {
            os << "usage:\n  " << procName << " ";
            argSynopsis( os );
            if( !m_options.empty() ) {
                os << " [options]\n\nwhere options are: \n";
                optUsage( os, 2 );
            }
            os << "\n";
        }
        std::string usage( std::string const& procName ) const {
            std::ostringstream oss;
            usage( oss, procName );
            return oss.str();
        }

        std::vector<Parser::Token> parseInto( int argc, char const * const * argv, ConfigT& config ) const {
            std::string processName = argv[0];
            std::size_t lastSlash = processName.find_last_of( "/\\" );
            if( lastSlash != std::string::npos )
                processName = processName.substr( lastSlash+1 );
            m_boundProcessName.set( config, processName );
            std::vector<Parser::Token> tokens;
            Parser parser;
            parser.parseIntoTokens( argc, argv, tokens );
            return populate( tokens, config );
        }

        std::vector<Parser::Token> populate( std::vector<Parser::Token> const& tokens, ConfigT& config ) const {
            if( m_options.empty() && m_positionalArgs.empty() )
                throw std::logic_error( "No options or arguments specified" );

            std::vector<Parser::Token> unusedTokens = populateOptions( tokens, config );
            unusedTokens = populateFixedArgs( unusedTokens, config );
            unusedTokens = populateFloatingArgs( unusedTokens, config );
            return unusedTokens;
        }

        std::vector<Parser::Token> populateOptions( std::vector<Parser::Token> const& tokens, ConfigT& config ) const {
            std::vector<Parser::Token> unusedTokens;
            std::vector<std::string> errors;
            for( std::size_t i = 0; i < tokens.size(); ++i ) {
                Parser::Token const& token = tokens[i];
                typename std::vector<Arg>::const_iterator it = m_options.begin(), itEnd = m_options.end();
                for(; it != itEnd; ++it ) {
                    Arg const& arg = *it;

                    try {
                        if( ( token.type == Parser::Token::ShortOpt && arg.hasShortName( token.data ) ) ||
                            ( token.type == Parser::Token::LongOpt && arg.hasLongName( token.data ) ) ) {
                            if( arg.takesArg() ) {
                                if( i == tokens.size()-1 || tokens[i+1].type != Parser::Token::Positional )
                                    errors.push_back( "Expected argument to option: " + token.data );
                                else
                                    arg.boundField.set( config, tokens[++i].data );
                            }
                            else {
                                arg.boundField.setFlag( config );
                            }
                            break;
                        }
                    }
                    catch( std::exception& ex ) {
                        errors.push_back( std::string( ex.what() ) + "\n- while parsing: (" + arg.commands() + ")" );
                    }
                }
                if( it == itEnd ) {
                    if( token.type == Parser::Token::Positional || !m_throwOnUnrecognisedTokens )
                        unusedTokens.push_back( token );
                    else if( m_throwOnUnrecognisedTokens )
                        errors.push_back( "unrecognised option: " + token.data );
                }
            }
            if( !errors.empty() ) {
                std::ostringstream oss;
                for( std::vector<std::string>::const_iterator it = errors.begin(), itEnd = errors.end();
                        it != itEnd;
                        ++it ) {
                    if( it != errors.begin() )
                        oss << "\n";
                    oss << *it;
                }
                throw std::runtime_error( oss.str() );
            }
            return unusedTokens;
        }
        std::vector<Parser::Token> populateFixedArgs( std::vector<Parser::Token> const& tokens, ConfigT& config ) const {
            std::vector<Parser::Token> unusedTokens;
            int position = 1;
            for( std::size_t i = 0; i < tokens.size(); ++i ) {
                Parser::Token const& token = tokens[i];
                typename std::map<int, Arg>::const_iterator it = m_positionalArgs.find( position );
                if( it != m_positionalArgs.end() )
                    it->second.boundField.set( config, token.data );
                else
                    unusedTokens.push_back( token );
                if( token.type == Parser::Token::Positional )
                    position++;
            }
            return unusedTokens;
        }
        std::vector<Parser::Token> populateFloatingArgs( std::vector<Parser::Token> const& tokens, ConfigT& config ) const {
            if( !m_arg.get() )
                return tokens;
            std::vector<Parser::Token> unusedTokens;
            for( std::size_t i = 0; i < tokens.size(); ++i ) {
                Parser::Token const& token = tokens[i];
                if( token.type == Parser::Token::Positional )
                    m_arg->boundField.set( config, token.data );
                else
                    unusedTokens.push_back( token );
            }
            return unusedTokens;
        }

    private:
        Detail::BoundArgFunction<ConfigT> m_boundProcessName;
        std::vector<Arg> m_options;
        std::map<int, Arg> m_positionalArgs;
        ArgAutoPtr m_arg;
        int m_highestSpecifiedArgPosition;
        bool m_throwOnUnrecognisedTokens;
    };

} // end namespace Clara

#ifdef CLICHE_CLARA_OUTER_NAMESPACE
} // end outer namespace
#endif

#endif // TWOBLUECUBES_CLARA_H_ALREADY_INCLUDED

