/*
 *  Created by Phil on 2/12/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAGS_H_INCLUDED
#define TWOBLUECUBES_CATCH_TAGS_H_INCLUDED

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

        void parse( std::string const& str );

    protected:
        virtual void acceptTag( std::string const& tag ) = 0;
        virtual void acceptChar( char c ) = 0;
        virtual void endParse() {}

    private:
    };

    class TagExtracter : public TagParser {
    public:

        TagExtracter( std::set<std::string>& tags );
        virtual ~TagExtracter();

        void parse( std::string& description );

    private:
        virtual void acceptTag( std::string const& tag );
        virtual void acceptChar( char c );

        TagExtracter& operator=(TagExtracter const&);

        std::set<std::string>& m_tags;
        std::string m_remainder;
    };

    class Tag {
    public:
        Tag();
        Tag( std::string const& name, bool isNegated );
        std::string getName() const;
        bool isNegated() const;
        bool operator ! () const;

    private:
        std::string m_name;
        bool m_isNegated;
    };

    class TagSet {
        typedef std::map<std::string, Tag> TagMap;
    public:
        void add( Tag const& tag );
        bool empty() const;
        bool matches( std::set<std::string> const& tags ) const;

    private:
        TagMap m_tags;
    };


    class TagExpression {
    public:
        bool matches( std::set<std::string> const& tags ) const;

    private:
        friend class TagExpressionParser;

        std::vector<TagSet> m_tagSets;
    };

    class TagExpressionParser : public TagParser {
    public:
        TagExpressionParser( TagExpression& exp );
        ~TagExpressionParser();

    private:
        virtual void acceptTag( std::string const& tag );
        virtual void acceptChar( char c );
        virtual void endParse();

        TagExpressionParser& operator=(TagExpressionParser const&);

        bool m_isNegated;
        TagSet m_currentTagSet;
        TagExpression& m_exp;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED
