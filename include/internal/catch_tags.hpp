/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED

#include "catch_tags.h"

namespace Catch {
    INTERNAL_CATCH_INLINE TagParser::~TagParser() {}

    INTERNAL_CATCH_INLINE void TagParser::parse( std::string const& str ) {
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

    INTERNAL_CATCH_INLINE TagExtracter::TagExtracter( std::set<std::string>& tags )
    :   m_tags( tags )
    {}

    INTERNAL_CATCH_INLINE TagExtracter::~TagExtracter() {}

    INTERNAL_CATCH_INLINE void TagExtracter::parse( std::string& description ) {
        TagParser::parse( description );
        description = m_remainder;
    }

    INTERNAL_CATCH_INLINE void TagExtracter::acceptTag( std::string const& tag ) {
        m_tags.insert( toLower( tag ) );
    }
    INTERNAL_CATCH_INLINE void TagExtracter::acceptChar( char c ) {
        m_remainder += c;
    }

    INTERNAL_CATCH_INLINE Tag::Tag() : m_isNegated( false ) {}
    INTERNAL_CATCH_INLINE Tag::Tag( std::string const& name, bool isNegated )
    :   m_name( name ),
        m_isNegated( isNegated )
    {}

    INTERNAL_CATCH_INLINE std::string Tag::getName() const {
        return m_name;
    }
    INTERNAL_CATCH_INLINE bool Tag::isNegated() const {
        return m_isNegated;
    }

    INTERNAL_CATCH_INLINE bool Tag::operator ! () const {
        return m_name.empty();
    }


    INTERNAL_CATCH_INLINE void TagSet::add( Tag const& tag ) {
        m_tags.insert( std::make_pair( toLower( tag.getName() ), tag ) );
    }

    INTERNAL_CATCH_INLINE bool TagSet::empty() const {
        return m_tags.empty();
    }

    INTERNAL_CATCH_INLINE bool TagSet::matches( std::set<std::string> const& tags ) const {
        for(    TagMap::const_iterator
                    it = m_tags.begin(), itEnd = m_tags.end();
                it != itEnd;
                ++it ) {
            bool found = tags.find( it->first ) != tags.end();
            if( found == it->second.isNegated() )
                return false;
        }
        return true;
    }


    INTERNAL_CATCH_INLINE bool TagExpression::matches( std::set<std::string> const& tags ) const {
        for(    std::vector<TagSet>::const_iterator
                    it = m_tagSets.begin(), itEnd = m_tagSets.end();
                it != itEnd;
                ++it )
            if( it->matches( tags ) )
                return true;
        return false;
    }

    INTERNAL_CATCH_INLINE TagExpressionParser::TagExpressionParser( TagExpression& exp )
    :   m_isNegated( false ),
        m_exp( exp )
    {}

    INTERNAL_CATCH_INLINE TagExpressionParser::~TagExpressionParser() {}

    INTERNAL_CATCH_INLINE void TagExpressionParser::acceptTag( std::string const& tag ) {
        m_currentTagSet.add( Tag( tag, m_isNegated ) );
        m_isNegated = false;
    }

    INTERNAL_CATCH_INLINE void TagExpressionParser::acceptChar( char c ) {
        switch( c ) {
            case '~':
                m_isNegated = true;
                break;
            case ',':
                m_exp.m_tagSets.push_back( m_currentTagSet );
                m_currentTagSet = TagSet();
                break;
        }
    }

    INTERNAL_CATCH_INLINE void TagExpressionParser::endParse() {
        if( !m_currentTagSet.empty() )
            m_exp.m_tagSets.push_back( m_currentTagSet );
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED
