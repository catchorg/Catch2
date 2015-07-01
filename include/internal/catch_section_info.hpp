/*
 *  Created by Phil Nash on 4/5/2012
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SECTION_INFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SECTION_INFO_HPP_INCLUDED

#include "catch_common.h"

#include <map>
#include <string>

namespace Catch {

    class RunningSection {
    public:

        typedef std::vector<RunningSection*> SubSections;

        enum State {
            Root,
            Unknown,
            Branch,
            TestedBranch,
            TestedLeaf
        };

        RunningSection( RunningSection* parent, std::string const& name )
        :   m_state( Unknown ),
            m_parent( parent ),
            m_name( name )
        {}

        RunningSection( std::string const& name )
        :   m_state( Root ),
            m_parent( CATCH_NULL ),
            m_name( name )
        {}

        ~RunningSection() {
            deleteAll( m_subSections );
        }

        std::string getName() const {
            return m_name;
        }

        bool shouldRun() const {
            return m_state < TestedBranch;
        }

        bool isBranch() const {
            return m_state == Branch;
        }

        const RunningSection* getParent() const {
            return m_parent;
        }

        bool hasUntestedSections() const {
            if( m_state == Unknown )
                return true;
            for(    SubSections::const_iterator it = m_subSections.begin();
                    it != m_subSections.end();
                    ++it)
                if( (*it)->hasUntestedSections() )
                    return true;
            return false;
        }

        // Mutable methods:

        RunningSection* getParent() {
            return m_parent;
        }

        RunningSection* findOrAddSubSection( std::string const& name, bool& changed ) {
            for(    SubSections::const_iterator it = m_subSections.begin();
                    it != m_subSections.end();
                    ++it)
                if( (*it)->getName() == name )
                    return *it;
            RunningSection* subSection = new RunningSection( this, name );
            m_subSections.push_back( subSection );
            m_state = Branch;
            changed = true;
            return subSection;
        }

        bool ran() {
            if( m_state >= Branch )
                return false;
            m_state = TestedLeaf;
            return true;
        }

        void ranToCompletion() {
            if( m_state == Branch && !hasUntestedSections() )
                m_state = TestedBranch;
        }

    private:
        State m_state;
        RunningSection* m_parent;
        std::string m_name;
        SubSections m_subSections;
    };
}

#endif // TWOBLUECUBES_CATCH_SECTION_INFO_HPP_INCLUDED
