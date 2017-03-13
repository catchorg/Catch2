/*
 *  Created by Phil on 25/05/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Version 0.0.2.4

// Only use header guard if we are not using an outer namespace
#if !defined(TWOBLUECUBES_CLARA_H_INCLUDED) || defined(STITCH_CLARA_OPEN_NAMESPACE)

#ifndef STITCH_CLARA_OPEN_NAMESPACE
#define TWOBLUECUBES_CLARA_H_INCLUDED
#define STITCH_CLARA_OPEN_NAMESPACE
#define STITCH_CLARA_CLOSE_NAMESPACE
#else
#define STITCH_CLARA_CLOSE_NAMESPACE }
#endif


#define STITCH_TBC_TEXT_FORMAT_OPEN_NAMESPACE STITCH_CLARA_OPEN_NAMESPACE

// ----------- #included from tbc_text_format.h -----------

/*
 *  Created by Phil on 18/4/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
// Only use header guard if we are not using an outer namespace
#if !defined(TBC_TEXT_FORMAT_H_INCLUDED) || defined(STITCH_TBC_TEXT_FORMAT_OUTER_NAMESPACE)
#ifndef STITCH_TBC_TEXT_FORMAT_OUTER_NAMESPACE
#define TBC_TEXT_FORMAT_H_INCLUDED
#endif

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

// Use optional outer namespace
#ifdef STITCH_TBC_TEXT_FORMAT_OUTER_NAMESPACE
namespace STITCH_TBC_TEXT_FORMAT_OUTER_NAMESPACE {
#endif

namespace Tbc {

#ifdef TBC_TEXT_FORMAT_CONSOLE_WIDTH
    const unsigned int consoleWidth = TBC_TEXT_FORMAT_CONSOLE_WIDTH;
#else
    const unsigned int consoleWidth = 80;
#endif

    struct TextAttributes {
        TextAttributes()
        :   initialIndent( std::string::npos ),
            indent( 0 ),
            width( consoleWidth-1 ),
            tabChar( '\t' )
        {}

        TextAttributes& setInitialIndent( std::size_t _value )  { initialIndent = _value; return *this; }
        TextAttributes& setIndent( std::size_t _value )         { indent = _value; return *this; }
        TextAttributes& setWidth( std::size_t _value )          { width = _value; return *this; }
        TextAttributes& setTabChar( char _value )               { tabChar = _value; return *this; }

        std::size_t initialIndent;  // indent of first line, or npos
        std::size_t indent;         // indent of subsequent lines, or all if initialIndent is npos
        std::size_t width;          // maximum width of text, including indent. Longer text will wrap
        char tabChar;               // If this char is seen the indent is changed to current pos
    };

    class Text {
    public:
        Text( std::string const& _str, TextAttributes const& _attr = TextAttributes() )
        : attr( _attr )
        {
            std::string wrappableChars = " [({.,/|\\-";
            std::size_t indent = _attr.initialIndent != std::string::npos
                ? _attr.initialIndent
                : _attr.indent;
            std::string remainder = _str;

            while( !remainder.empty() ) {
                if( lines.size() >= 1000 ) {
                    lines.push_back( "... message truncated due to excessive size" );
                    return;
                }
                std::size_t tabPos = std::string::npos;
                std::size_t width = (std::min)( remainder.size(), _attr.width - indent );
                std::size_t pos = remainder.find_first_of( '\n' );
                if( pos <= width ) {
                    width = pos;
                }
                pos = remainder.find_last_of( _attr.tabChar, width );
                if( pos != std::string::npos ) {
                    tabPos = pos;
                    if( remainder[width] == '\n' )
                        width--;
                    remainder = remainder.substr( 0, tabPos ) + remainder.substr( tabPos+1 );
                }

                if( width == remainder.size() ) {
                    spliceLine( indent, remainder, width );
                }
                else if( remainder[width] == '\n' ) {
                    spliceLine( indent, remainder, width );
                    if( width <= 1 || remainder.size() != 1 )
                        remainder = remainder.substr( 1 );
                    indent = _attr.indent;
                }
                else {
                    pos = remainder.find_last_of( wrappableChars, width );
                    if( pos != std::string::npos && pos > 0 ) {
                        spliceLine( indent, remainder, pos );
                        if( remainder[0] == ' ' )
                            remainder = remainder.substr( 1 );
                    }
                    else {
                        spliceLine( indent, remainder, width-1 );
                        lines.back() += "-";
                    }
                    if( lines.size() == 1 )
                        indent = _attr.indent;
                    if( tabPos != std::string::npos )
                        indent += tabPos;
                }
            }
        }

        void spliceLine( std::size_t _indent, std::string& _remainder, std::size_t _pos ) {
            lines.push_back( std::string( _indent, ' ' ) + _remainder.substr( 0, _pos ) );
            _remainder = _remainder.substr( _pos );
        }

        typedef std::vector<std::string>::const_iterator const_iterator;

        const_iterator begin() const { return lines.begin(); }
        const_iterator end() const { return lines.end(); }
        std::string const& last() const { return lines.back(); }
        std::size_t size() const { return lines.size(); }
        std::string const& operator[]( std::size_t _index ) const { return lines[_index]; }
        std::string toString() const {
            std::ostringstream oss;
            oss << *this;
            return oss.str();
        }

        inline friend std::ostream& operator << ( std::ostream& _stream, Text const& _text ) {
            for( Text::const_iterator it = _text.begin(), itEnd = _text.end();
                it != itEnd; ++it ) {
                if( it != _text.begin() )
                    _stream << "\n";
                _stream << *it;
            }
            return _stream;
        }

    private:
        std::string str;
        TextAttributes attr;
        std::vector<std::string> lines;
    };

} // end namespace Tbc

#ifdef STITCH_TBC_TEXT_FORMAT_OUTER_NAMESPACE
} // end outer namespace
#endif

#endif // TBC_TEXT_FORMAT_H_INCLUDED

// ----------- end of #include from tbc_text_format.h -----------
// ........... back in clara.h

#undef STITCH_TBC_TEXT_FORMAT_OPEN_NAMESPACE


// ----------- #included from clara_compilers.h -----------

/*
 *  Created by Phil on 10/02/2016.
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CLARA_COMPILERS_H_INCLUDED
#define TWOBLUECUBES_CLARA_COMPILERS_H_INCLUDED

// Detect a number of compiler features - mostly C++11/14 conformance - by compiler
// The following features are defined:
//
// CLARA_CONFIG_CPP11_NULLPTR : is nullptr supported?
// CLARA_CONFIG_CPP11_NOEXCEPT : is noexcept supported?
// CLARA_CONFIG_CPP11_GENERATED_METHODS : The delete and default keywords for compiler generated methods
// CLARA_CONFIG_CPP11_OVERRIDE : is override supported?
// CLARA_CONFIG_CPP11_UNIQUE_PTR : is unique_ptr supported (otherwise use auto_ptr)

// CLARA_CONFIG_CPP11_OR_GREATER : Is C++11 supported?

// CLARA_CONFIG_VARIADIC_MACROS : are variadic macros supported?

// In general each macro has a _NO_<feature name> form
// (e.g. CLARA_CONFIG_CPP11_NO_NULLPTR) which disables the feature.
// Many features, at point of detection, define an _INTERNAL_ macro, so they
// can be combined, en-mass, with the _NO_ forms later.

// All the C++11 features can be disabled with CLARA_CONFIG_NO_CPP11

#ifdef __clang__

#if __has_feature(cxx_nullptr)
#define CLARA_INTERNAL_CONFIG_CPP11_NULLPTR
#endif

#if __has_feature(cxx_noexcept)
#define CLARA_INTERNAL_CONFIG_CPP11_NOEXCEPT
#endif

#endif // __clang__

////////////////////////////////////////////////////////////////////////////////
// GCC
#ifdef __GNUC__

#if __GNUC__ == 4 && __GNUC_MINOR__ >= 6 && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define CLARA_INTERNAL_CONFIG_CPP11_NULLPTR
#endif

// - otherwise more recent versions define __cplusplus >= 201103L
// and will get picked up below

#endif // __GNUC__

////////////////////////////////////////////////////////////////////////////////
// Visual C++
#ifdef _MSC_VER

#if (_MSC_VER >= 1600)
#define CLARA_INTERNAL_CONFIG_CPP11_NULLPTR
#define CLARA_INTERNAL_CONFIG_CPP11_UNIQUE_PTR
#endif

#if (_MSC_VER >= 1900 ) // (VC++ 13 (VS2015))
#define CLARA_INTERNAL_CONFIG_CPP11_NOEXCEPT
#define CLARA_INTERNAL_CONFIG_CPP11_GENERATED_METHODS
#endif

#endif // _MSC_VER


////////////////////////////////////////////////////////////////////////////////
// C++ language feature support

// catch all support for C++11
#if defined(__cplusplus) && __cplusplus >= 201103L

#define CLARA_CPP11_OR_GREATER

#if !defined(CLARA_INTERNAL_CONFIG_CPP11_NULLPTR)
#define CLARA_INTERNAL_CONFIG_CPP11_NULLPTR
#endif

#ifndef CLARA_INTERNAL_CONFIG_CPP11_NOEXCEPT
#define CLARA_INTERNAL_CONFIG_CPP11_NOEXCEPT
#endif

#ifndef CLARA_INTERNAL_CONFIG_CPP11_GENERATED_METHODS
#define CLARA_INTERNAL_CONFIG_CPP11_GENERATED_METHODS
#endif

#if !defined(CLARA_INTERNAL_CONFIG_CPP11_OVERRIDE)
#define CLARA_INTERNAL_CONFIG_CPP11_OVERRIDE
#endif
#if !defined(CLARA_INTERNAL_CONFIG_CPP11_UNIQUE_PTR)
#define CLARA_INTERNAL_CONFIG_CPP11_UNIQUE_PTR
#endif


#endif // __cplusplus >= 201103L

// Now set the actual defines based on the above + anything the user has configured
#if defined(CLARA_INTERNAL_CONFIG_CPP11_NULLPTR) && !defined(CLARA_CONFIG_CPP11_NO_NULLPTR) && !defined(CLARA_CONFIG_CPP11_NULLPTR) && !defined(CLARA_CONFIG_NO_CPP11)
#define CLARA_CONFIG_CPP11_NULLPTR
#endif
#if defined(CLARA_INTERNAL_CONFIG_CPP11_NOEXCEPT) && !defined(CLARA_CONFIG_CPP11_NO_NOEXCEPT) && !defined(CLARA_CONFIG_CPP11_NOEXCEPT) && !defined(CLARA_CONFIG_NO_CPP11)
#define CLARA_CONFIG_CPP11_NOEXCEPT
#endif
#if defined(CLARA_INTERNAL_CONFIG_CPP11_GENERATED_METHODS) && !defined(CLARA_CONFIG_CPP11_NO_GENERATED_METHODS) && !defined(CLARA_CONFIG_CPP11_GENERATED_METHODS) && !defined(CLARA_CONFIG_NO_CPP11)
#define CLARA_CONFIG_CPP11_GENERATED_METHODS
#endif
#if defined(CLARA_INTERNAL_CONFIG_CPP11_OVERRIDE) && !defined(CLARA_CONFIG_NO_OVERRIDE) && !defined(CLARA_CONFIG_CPP11_OVERRIDE) && !defined(CLARA_CONFIG_NO_CPP11)
#define CLARA_CONFIG_CPP11_OVERRIDE
#endif
#if defined(CLARA_INTERNAL_CONFIG_CPP11_UNIQUE_PTR) && !defined(CLARA_CONFIG_NO_UNIQUE_PTR) && !defined(CLARA_CONFIG_CPP11_UNIQUE_PTR) && !defined(CLARA_CONFIG_NO_CPP11)
#define CLARA_CONFIG_CPP11_UNIQUE_PTR
#endif


// noexcept support:
#if defined(CLARA_CONFIG_CPP11_NOEXCEPT) && !defined(CLARA_NOEXCEPT)
#define CLARA_NOEXCEPT noexcept
#  define CLARA_NOEXCEPT_IS(x) noexcept(x)
#else
#define CLARA_NOEXCEPT throw()
#  define CLARA_NOEXCEPT_IS(x)
#endif

// nullptr support
#ifdef CLARA_CONFIG_CPP11_NULLPTR
#define CLARA_NULL nullptr
#else
#define CLARA_NULL NULL
#endif

// override support
#ifdef CLARA_CONFIG_CPP11_OVERRIDE
#define CLARA_OVERRIDE override
#else
#define CLARA_OVERRIDE
#endif

// unique_ptr support
#ifdef CLARA_CONFIG_CPP11_UNIQUE_PTR
#   define CLARA_AUTO_PTR( T ) std::unique_ptr<T>
#else
#   define CLARA_AUTO_PTR( T ) std::auto_ptr<T>
#endif

#endif // TWOBLUECUBES_CLARA_COMPILERS_H_INCLUDED


// ----------- end of #include from clara_compilers.h -----------
// ........... back in clara.h


#include <map>
#include <stdexcept>
#include <memory>

#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#define CLARA_PLATFORM_WINDOWS
#endif


// Use optional outer namespace
#ifdef STITCH_CLARA_OPEN_NAMESPACE
STITCH_CLARA_OPEN_NAMESPACE
#endif

namespace Clara {

    struct UnpositionalTag {};

    extern UnpositionalTag _;

#ifdef CLARA_CONFIG_MAIN
    UnpositionalTag _;
#endif

    namespace Detail {

#ifdef CLARA_CONSOLE_WIDTH
    const unsigned int consoleWidth = CLARA_CONFIG_CONSOLE_WIDTH;
#else
    const unsigned int consoleWidth = 80;
#endif

        using namespace Tbc;

        inline bool startsWith( std::string const& str, std::string const& prefix ) {
            return str.size() >= prefix.size() && str.substr( 0, prefix.size() ) == prefix;
        }

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
        char toLowerCh(char c) {
            return static_cast<char>( std::tolower( c ) );
        }
        inline void convertInto( std::string const& _source, bool& _dest ) {
            std::string sourceLC = _source;
            std::transform( sourceLC.begin(), sourceLC.end(), sourceLC.begin(), toLowerCh );
            if( sourceLC == "y" || sourceLC == "1" || sourceLC == "true" || sourceLC == "yes" || sourceLC == "on" )
                _dest = true;
            else if( sourceLC == "n" || sourceLC == "0" || sourceLC == "false" || sourceLC == "no" || sourceLC == "off" )
                _dest = false;
            else
                throw std::runtime_error( "Expected a boolean value but did not recognise:\n  '" + _source + "'" );
        }


        template<typename ConfigT>
        struct IArgFunction {
            virtual ~IArgFunction() {}
#ifdef CLARA_CONFIG_CPP11_GENERATED_METHODS
            IArgFunction()                      = default;
            IArgFunction( IArgFunction const& ) = default;
#endif
            virtual void set( ConfigT& config, std::string const& value ) const = 0;
            virtual bool takesArg() const = 0;
            virtual IArgFunction* clone() const = 0;
        };

        template<typename ConfigT>
        class BoundArgFunction {
        public:
            BoundArgFunction() : functionObj( CLARA_NULL ) {}
            BoundArgFunction( IArgFunction<ConfigT>* _functionObj ) : functionObj( _functionObj ) {}
            BoundArgFunction( BoundArgFunction const& other ) : functionObj( other.functionObj ? other.functionObj->clone() : CLARA_NULL ) {}
            BoundArgFunction& operator = ( BoundArgFunction const& other ) {
                IArgFunction<ConfigT>* newFunctionObj = other.functionObj ? other.functionObj->clone() : CLARA_NULL;
                delete functionObj;
                functionObj = newFunctionObj;
                return *this;
            }
            ~BoundArgFunction() { delete functionObj; }

            void set( ConfigT& config, std::string const& value ) const {
                functionObj->set( config, value );
            }
            bool takesArg() const { return functionObj->takesArg(); }

            bool isSet() const {
                return functionObj != CLARA_NULL;
            }
        private:
            IArgFunction<ConfigT>* functionObj;
        };


        template<typename C>
        struct NullBinder : IArgFunction<C>{
            virtual void set( C&, std::string const& ) const {}
            virtual bool takesArg() const { return true; }
            virtual IArgFunction<C>* clone() const { return new NullBinder( *this ); }
        };

        template<typename C, typename M>
        struct BoundDataMember : IArgFunction<C>{
            BoundDataMember( M C::* _member ) : member( _member ) {}
            virtual void set( C& p, std::string const& stringValue ) const {
                convertInto( stringValue, p.*member );
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
            virtual bool takesArg() const { return !IsBool<T>::value; }
            virtual IArgFunction<C>* clone() const { return new BoundBinaryFunction( *this ); }
            void (*function)( C&, T );
        };

    } // namespace Detail

    inline std::vector<std::string> argsToVector( int argc, char const* const* const argv ) {
        std::vector<std::string> args( static_cast<std::size_t>( argc ) );
        for( std::size_t i = 0; i < static_cast<std::size_t>( argc ); ++i )
            args[i] = argv[i];

        return args;
    }

    class Parser {
        enum Mode { None, MaybeShortOpt, SlashOpt, ShortOpt, LongOpt, Positional };
        Mode mode;
        std::size_t from;
        bool inQuotes;
    public:

        struct Token {
            enum Type { Positional, ShortOpt, LongOpt };
            Token( Type _type, std::string const& _data ) : type( _type ), data( _data ) {}
            Type type;
            std::string data;
        };

        Parser() : mode( None ), from( 0 ), inQuotes( false ){}

        void parseIntoTokens( std::vector<std::string> const& args, std::vector<Token>& tokens ) {
            const std::string doubleDash = "--";
            for( std::size_t i = 1; i < args.size() && args[i] != doubleDash; ++i )
                parseIntoTokens( args[i], tokens);
        }

        void parseIntoTokens( std::string const& arg, std::vector<Token>& tokens ) {
            for( std::size_t i = 0; i < arg.size(); ++i ) {
                char c = arg[i];
                if( c == '"' )
                    inQuotes = !inQuotes;
                mode = handleMode( i, c, arg, tokens );
            }
            mode = handleMode( arg.size(), '\0', arg, tokens );
        }
        Mode handleMode( std::size_t i, char c, std::string const& arg, std::vector<Token>& tokens ) {
            switch( mode ) {
                case None: return handleNone( i, c );
                case MaybeShortOpt: return handleMaybeShortOpt( i, c );
                case ShortOpt:
                case LongOpt:
                case SlashOpt: return handleOpt( i, c, arg, tokens );
                case Positional: return handlePositional( i, c, arg, tokens );
                default: throw std::logic_error( "Unknown mode" );
            }
        }

        Mode handleNone( std::size_t i, char c ) {
            if( inQuotes ) {
                from = i;
                return Positional;
            }
            switch( c ) {
                case '-': return MaybeShortOpt;
#ifdef CLARA_PLATFORM_WINDOWS
                case '/': from = i+1; return SlashOpt;
#endif
                default: from = i; return Positional;
            }
        }
        Mode handleMaybeShortOpt( std::size_t i, char c ) {
            switch( c ) {
                case '-': from = i+1; return LongOpt;
                default: from = i; return ShortOpt;
            }
        }

        Mode handleOpt( std::size_t i, char c, std::string const& arg, std::vector<Token>& tokens ) {
            if( std::string( ":=\0", 3 ).find( c ) == std::string::npos )
                return mode;

            std::string optName = arg.substr( from, i-from );
            if( mode == ShortOpt )
                for( std::size_t j = 0; j < optName.size(); ++j )
                    tokens.push_back( Token( Token::ShortOpt, optName.substr( j, 1 ) ) );
            else if( mode == SlashOpt && optName.size() == 1 )
                tokens.push_back( Token( Token::ShortOpt, optName ) );
            else
                tokens.push_back( Token( Token::LongOpt, optName ) );
            return None;
        }
        Mode handlePositional( std::size_t i, char c, std::string const& arg, std::vector<Token>& tokens ) {
            if( inQuotes || std::string( "\0", 1 ).find( c ) == std::string::npos )
                return mode;

            std::string data = arg.substr( from, i-from );
            tokens.push_back( Token( Token::Positional, data ) );
            return None;
        }
    };

    template<typename ConfigT>
    struct CommonArgProperties {
        CommonArgProperties() {}
        CommonArgProperties( Detail::BoundArgFunction<ConfigT> const& _boundField ) : boundField( _boundField ) {}

        Detail::BoundArgFunction<ConfigT> boundField;
        std::string description;
        std::string detail;
        std::string placeholder; // Only value if boundField takes an arg

        bool takesArg() const {
            return !placeholder.empty();
        }
        void validate() const {
            if( !boundField.isSet() )
                throw std::logic_error( "option not bound" );
        }
    };
    struct OptionArgProperties {
        std::vector<std::string> shortNames;
        std::string longName;

        bool hasShortName( std::string const& shortName ) const {
            return std::find( shortNames.begin(), shortNames.end(), shortName ) != shortNames.end();
        }
        bool hasLongName( std::string const& _longName ) const {
            return _longName == longName;
        }
    };
    struct PositionalArgProperties {
        PositionalArgProperties() : position( -1 ) {}
        int position; // -1 means non-positional (floating)

        bool isFixedPositional() const {
            return position != -1;
        }
    };

    template<typename ConfigT>
    class CommandLine {

        struct Arg : CommonArgProperties<ConfigT>, OptionArgProperties, PositionalArgProperties {
            Arg() {}
            Arg( Detail::BoundArgFunction<ConfigT> const& _boundField ) : CommonArgProperties<ConfigT>( _boundField ) {}

            using CommonArgProperties<ConfigT>::placeholder; // !TBD

            std::string dbgName() const {
                if( !longName.empty() )
                    return "--" + longName;
                if( !shortNames.empty() )
                    return "-" + shortNames[0];
                return "positional args";
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
                if( !placeholder.empty() )
                    oss << " <" << placeholder << ">";
                return oss.str();
            }
        };

        typedef CLARA_AUTO_PTR( Arg ) ArgAutoPtr;

        friend void addOptName( Arg& arg, std::string const& optName )
        {
            if( optName.empty() )
                return;
            if( Detail::startsWith( optName, "--" ) ) {
                if( !arg.longName.empty() )
                    throw std::logic_error( "Only one long opt may be specified. '"
                        + arg.longName
                        + "' already specified, now attempting to add '"
                        + optName + "'" );
                arg.longName = optName.substr( 2 );
            }
            else if( Detail::startsWith( optName, "-" ) )
                arg.shortNames.push_back( optName.substr( 1 ) );
            else
                throw std::logic_error( "option must begin with - or --. Option was: '" + optName + "'" );
        }
        friend void setPositionalArg( Arg& arg, int position )
        {
            arg.position = position;
        }


        class ArgBuilder {
        public:
            ArgBuilder( Arg* arg ) : m_arg( arg ) {}

            // Bind a non-boolean data member (requires placeholder string)
            template<typename C, typename M>
            void bind( M C::* field, std::string const& placeholder ) {
                m_arg->boundField = new Detail::BoundDataMember<C,M>( field );
                m_arg->placeholder = placeholder;
            }
            // Bind a boolean data member (no placeholder required)
            template<typename C>
            void bind( bool C::* field ) {
                m_arg->boundField = new Detail::BoundDataMember<C,bool>( field );
            }

            // Bind a method taking a single, non-boolean argument (requires a placeholder string)
            template<typename C, typename M>
            void bind( void (C::* unaryMethod)( M ), std::string const& placeholder ) {
                m_arg->boundField = new Detail::BoundUnaryMethod<C,M>( unaryMethod );
                m_arg->placeholder = placeholder;
            }

            // Bind a method taking a single, boolean argument (no placeholder string required)
            template<typename C>
            void bind( void (C::* unaryMethod)( bool ) ) {
                m_arg->boundField = new Detail::BoundUnaryMethod<C,bool>( unaryMethod );
            }

            // Bind a method that takes no arguments (will be called if opt is present)
            template<typename C>
            void bind( void (C::* nullaryMethod)() ) {
                m_arg->boundField = new Detail::BoundNullaryMethod<C>( nullaryMethod );
            }

            // Bind a free function taking a single argument - the object to operate on (no placeholder string required)
            template<typename C>
            void bind( void (* unaryFunction)( C& ) ) {
                m_arg->boundField = new Detail::BoundUnaryFunction<C>( unaryFunction );
            }

            // Bind a free function taking a single argument - the object to operate on (requires a placeholder string)
            template<typename C, typename T>
            void bind( void (* binaryFunction)( C&, T ), std::string const& placeholder ) {
                m_arg->boundField = new Detail::BoundBinaryFunction<C, T>( binaryFunction );
                m_arg->placeholder = placeholder;
            }

            ArgBuilder& describe( std::string const& description ) {
                m_arg->description = description;
                return *this;
            }
            ArgBuilder& detail( std::string const& detail ) {
                m_arg->detail = detail;
                return *this;
            }

        protected:
            Arg* m_arg;
        };

        class OptBuilder : public ArgBuilder {
        public:
            OptBuilder( Arg* arg ) : ArgBuilder( arg ) {}
            OptBuilder( OptBuilder& other ) : ArgBuilder( other ) {}

            OptBuilder& operator[]( std::string const& optName ) {
                addOptName( *ArgBuilder::m_arg, optName );
                return *this;
            }
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
            if( other.m_floatingArg.get() )
                m_floatingArg.reset( new Arg( *other.m_floatingArg ) );
        }

        CommandLine& setThrowOnUnrecognisedTokens( bool shouldThrow = true ) {
            m_throwOnUnrecognisedTokens = shouldThrow;
            return *this;
        }


        OptBuilder operator[]( std::string const& optName ) {
            m_options.push_back( Arg() );
            addOptName( m_options.back(), optName );
            OptBuilder builder( &m_options.back() );
            return builder;
        }

        ArgBuilder operator[]( int position ) {
            m_positionalArgs.insert( std::make_pair( position, Arg() ) );
            if( position > m_highestSpecifiedArgPosition )
                m_highestSpecifiedArgPosition = position;
            setPositionalArg( m_positionalArgs[position], position );
            ArgBuilder builder( &m_positionalArgs[position] );
            return builder;
        }

        // Invoke this with the _ instance
        ArgBuilder operator[]( UnpositionalTag ) {
            if( m_floatingArg.get() )
                throw std::logic_error( "Only one unpositional argument can be added" );
            m_floatingArg.reset( new Arg() );
            ArgBuilder builder( m_floatingArg.get() );
            return builder;
        }

        template<typename C, typename M>
        void bindProcessName( M C::* field ) {
            m_boundProcessName = new Detail::BoundDataMember<C,M>( field );
        }
        template<typename C, typename M>
        void bindProcessName( void (C::*_unaryMethod)( M ) ) {
            m_boundProcessName = new Detail::BoundUnaryMethod<C,M>( _unaryMethod );
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
                Detail::Text desc( it->description, Detail::TextAttributes()
                                                        .setWidth( width - maxWidth - 3 ) );

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
                    os << "<" << it->second.placeholder << ">";
                else if( m_floatingArg.get() )
                    os << "<" << m_floatingArg->placeholder << ">";
                else
                    throw std::logic_error( "non consecutive positional arguments with no floating args" );
            }
            // !TBD No indication of mandatory args
            if( m_floatingArg.get() ) {
                if( m_highestSpecifiedArgPosition > 1 )
                    os << " ";
                os << "[<" << m_floatingArg->placeholder << "> ...]";
            }
        }
        std::string argSynopsis() const {
            std::ostringstream oss;
            argSynopsis( oss );
            return oss.str();
        }

        void usage( std::ostream& os, std::string const& procName ) const {
            validate();
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

        ConfigT parse( std::vector<std::string> const& args ) const {
            ConfigT config;
            parseInto( args, config );
            return config;
        }

        std::vector<Parser::Token> parseInto( std::vector<std::string> const& args, ConfigT& config ) const {
            std::string processName = args.empty() ? std::string() : args[0];
            std::size_t lastSlash = processName.find_last_of( "/\\" );
            if( lastSlash != std::string::npos )
                processName = processName.substr( lastSlash+1 );
            m_boundProcessName.set( config, processName );
            std::vector<Parser::Token> tokens;
            Parser parser;
            parser.parseIntoTokens( args, tokens );
            return populate( tokens, config );
        }

        std::vector<Parser::Token> populate( std::vector<Parser::Token> const& tokens, ConfigT& config ) const {
            validate();
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
                                arg.boundField.set( config, "true" );
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
                    else if( errors.empty() && m_throwOnUnrecognisedTokens )
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
            if( !m_floatingArg.get() )
                return tokens;
            std::vector<Parser::Token> unusedTokens;
            for( std::size_t i = 0; i < tokens.size(); ++i ) {
                Parser::Token const& token = tokens[i];
                if( token.type == Parser::Token::Positional )
                    m_floatingArg->boundField.set( config, token.data );
                else
                    unusedTokens.push_back( token );
            }
            return unusedTokens;
        }

        void validate() const
        {
            if( m_options.empty() && m_positionalArgs.empty() && !m_floatingArg.get() )
                throw std::logic_error( "No options or arguments specified" );

            for( typename std::vector<Arg>::const_iterator  it = m_options.begin(),
                                                            itEnd = m_options.end();
                    it != itEnd; ++it )
                it->validate();
        }

    private:
        Detail::BoundArgFunction<ConfigT> m_boundProcessName;
        std::vector<Arg> m_options;
        std::map<int, Arg> m_positionalArgs;
        ArgAutoPtr m_floatingArg;
        int m_highestSpecifiedArgPosition;
        bool m_throwOnUnrecognisedTokens;
    };

} // end namespace Clara


STITCH_CLARA_CLOSE_NAMESPACE
#undef STITCH_CLARA_OPEN_NAMESPACE
#undef STITCH_CLARA_CLOSE_NAMESPACE


#endif // TWOBLUECUBES_CLARA_H_INCLUDED
