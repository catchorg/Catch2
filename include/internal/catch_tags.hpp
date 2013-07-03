/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED

#include "catch_common.h"

#include <string>
#include <set>
#include <map>
#include <vector>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {
    class TagParser {
    public:
        virtual ~TagParser();

        void parse( std::string const& str ) {
            std::size_t pos = 0;
            while( pos < str.size() ) {
                char c = str[pos];
                if( c == '[' ) {
                    std::size_t end = str.find_first_of( ']', pos );
                    if( end != std::string::npos ) {
                        acceptTag( str.substr( pos+1, end-pos-1 ) );
                        pos = end+1;
                    }
                    else {
                        acceptChar( c );
                        pos++;
                    }
                }
                else {
                    acceptChar( c );
                    pos++;
                }
            }
            endParse();
        }

    protected:
        virtual void acceptTag( std::string const& tag ) = 0;
        virtual void acceptChar( char c ) = 0;
        virtual void endParse() {}

    private:
    };

    class TagExtracter : public TagParser {
    public:

        TagExtracter( std::set<std::string>& tags )
        :   m_tags( tags )
        {}
        virtual ~TagExtracter();

        void parse( std::string& description ) {
            TagParser::parse( description );
            description = m_remainder;
        }

    private:
        virtual void acceptTag( std::string const& tag ) {
            m_tags.insert( toLower( tag ) );
        }
        virtual void acceptChar( char c ) {
            m_remainder += c;
        }

        TagExtracter& operator=(TagExtracter const&);

        std::set<std::string>& m_tags;
        std::string m_remainder;
    };

    class Tag {
    public:
        Tag()
        :   m_isNegated( false )
        {}

        Tag( std::string const& name, bool isNegated )
        :   m_name( name ),
            m_isNegated( isNegated )
        {}

        std::string getName() const {
            return m_name;
        }
        bool isNegated() const {
            return m_isNegated;
        }

        bool operator ! () const {
            return m_name.empty();
        }

    private:
        std::string m_name;
        bool m_isNegated;
    };

    class TagSet {
        typedef std::map<std::string, Tag> TagMap;
    public:
        void add( Tag const& tag ) {
            m_tags.insert( std::make_pair( toLower( tag.getName() ), tag ) );
        }

        bool empty() const {
            return m_tags.empty();
        }

        bool matches( std::set<std::string> const& tags ) const {
            TagMap::const_iterator it = m_tags.begin();
            TagMap::const_iterator itEnd = m_tags.end();
            for(; it != itEnd; ++it ) {
                bool found = tags.find( it->first ) != tags.end();
                if( found == it->second.isNegated() )
                    return false;
            }
            return true;
        }

    private:
        TagMap m_tags;
    };

    class TagExpression {
    public:
        bool matches( std::set<std::string> const& tags ) const {
            std::vector<TagSet>::const_iterator it = m_tagSets.begin();
            std::vector<TagSet>::const_iterator itEnd = m_tagSets.end();
            for(; it != itEnd; ++it )
                if( it->matches( tags ) )
                    return true;
            return false;
        }

    private:
        friend class TagExpressionParser;

        std::vector<TagSet> m_tagSets;
    };

    class TagExpressionParser : public TagParser {
    public:
        TagExpressionParser( TagExpression& exp )
        :   m_isNegated( false ),
            m_exp( exp )
        {}

        ~TagExpressionParser();

    private:
        virtual void acceptTag( std::string const& tag ) {
            m_currentTagSet.add( Tag( tag, m_isNegated ) );
            m_isNegated = false;
        }
        virtual void acceptChar( char c ) {
            switch( c ) {
                case '~':
                    m_isNegated = true;
                    break;
                case ',':
                    m_exp.m_tagSets.push_back( m_currentTagSet );
                    break;
            }
        }
        virtual void endParse() {
            if( !m_currentTagSet.empty() )
                m_exp.m_tagSets.push_back( m_currentTagSet );
        }

        TagExpressionParser& operator=(TagExpressionParser const&);

        bool m_isNegated;
        TagSet m_currentTagSet;
        TagExpression& m_exp;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED
