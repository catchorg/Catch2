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
    TagParser::~TagParser() {}

    void TagParser::parse( std::string const& str ) {
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

    TagExtracter::TagExtracter( std::set<std::string>& tags )
    :   m_tags( tags )
    {}

    TagExtracter::~TagExtracter() {}

    void TagExtracter::parse( std::string& description ) {
        TagParser::parse( description );
        description = m_remainder;
    }

    void TagExtracter::acceptTag( std::string const& tag ) {
        m_tags.insert( toLower( tag ) );
    }
    void TagExtracter::acceptChar( char c ) {
        m_remainder += c;
    }

    Tag::Tag() : m_isNegated( false ) {}
    Tag::Tag( std::string const& name, bool isNegated )
    :   m_name( name ),
        m_isNegated( isNegated )
    {}

    std::string Tag::getName() const {
        return m_name;
    }
    bool Tag::isNegated() const {
        return m_isNegated;
    }

    bool Tag::operator ! () const {
        return m_name.empty();
    }


    void TagSet::add( Tag const& tag ) {
        m_tags.insert( std::make_pair( toLower( tag.getName() ), tag ) );
    }

    bool TagSet::empty() const {
        return m_tags.empty();
    }

    bool TagSet::matches( std::set<std::string> const& tags ) const {
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


    bool TagExpression::matches( std::set<std::string> const& tags ) const {
        for(    std::vector<TagSet>::const_iterator
                    it = m_tagSets.begin(), itEnd = m_tagSets.end();
                it != itEnd;
                ++it )
            if( it->matches( tags ) )
                return true;
        return false;
    }

    TagExpressionParser::TagExpressionParser( TagExpression& exp )
    :   m_isNegated( false ),
        m_exp( exp )
    {}

    TagExpressionParser::~TagExpressionParser() {}

    void TagExpressionParser::acceptTag( std::string const& tag ) {
        m_currentTagSet.add( Tag( tag, m_isNegated ) );
        m_isNegated = false;
    }

    void TagExpressionParser::acceptChar( char c ) {
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

    void TagExpressionParser::endParse() {
        if( !m_currentTagSet.empty() )
            m_exp.m_tagSets.push_back( m_currentTagSet );
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED
