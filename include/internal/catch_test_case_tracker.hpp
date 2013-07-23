/*
 *  Created by Phil Nash on 23/7/2013
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_TRACKER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_TRACKER_HPP_INCLUDED

#include <map>
#include <string>
#include <assert.h>

namespace Catch {
namespace SectionTracking {

    class TrackedSection {

        typedef std::map<std::string, TrackedSection> TrackedSections;

    public:
        enum RunState {
            NotStarted,
            Executing,
            ExecutingChildren,
            Completed
        };

        TrackedSection( std::string const& name, TrackedSection* parent )
        :   m_name( name ), m_runState( NotStarted ), m_parent( parent )
        {}

        RunState runState() const { return m_runState; }

        void addChild( std::string const& childName ) {
            m_children.insert( std::make_pair( childName, TrackedSection( childName, this ) ) );
        }
        TrackedSection* getChild( std::string const& childName ) {
            return &m_children.find( childName )->second;
        }

        void enter() {
            if( m_runState == NotStarted )
                m_runState = Executing;
        }
        void leave() {
            for( TrackedSections::const_iterator it = m_children.begin(), itEnd = m_children.end();
                    it != itEnd;
                    ++it )
                if( it->second.runState() != Completed ) {
                    m_runState = ExecutingChildren;
                    return;
                }
            m_runState = Completed;
        }
        TrackedSection* getParent() {
            return m_parent;
        }

    private:
        std::string m_name;
        RunState m_runState;
        TrackedSections m_children;
        TrackedSection* m_parent;
        
    };

    class SectionTracker {
    public:
        SectionTracker( TrackedSection& testCase )
        :   m_currentSection( &testCase ),
            m_completedASectionThisRun( false )
        {}
        
        bool enterSection( std::string const& name ) {
            if( m_completedASectionThisRun )
                return false;
            if( m_currentSection->runState() == TrackedSection::Executing ) {
                m_currentSection->addChild( name );
                return false;
            }
            else {
                TrackedSection* child = m_currentSection->getChild( name );
                if( child->runState() != TrackedSection::Completed ) {
                    m_currentSection = child;
                    m_currentSection->enter();
                    return true;
                }
                return false;
            }
        }

        void leaveSection() {
            m_currentSection->leave();
            m_currentSection = m_currentSection->getParent();
            assert( m_currentSection != NULL );
            m_completedASectionThisRun = true;
        }


    private:
        TrackedSection* m_currentSection;
        bool m_completedASectionThisRun;
    };

    class TestCaseTracker {
    public:

        TestCaseTracker( std::string const& testCaseName )
        :   m_testCase( testCaseName, NULL ),
            sections( m_testCase )
        {}

        void enter() {
            sections = SectionTracker( m_testCase );
            m_testCase.enter();
        }
        void leave() {
            m_testCase.leave();
        }

        bool enterSection( std::string const& name ) {
            return sections.enterSection( name );
        }
        void leaveSection() {
            sections.leaveSection();
        }
        
        bool isCompleted() const {
            return m_testCase.runState() == TrackedSection::Completed;
        }

    private:
        TrackedSection m_testCase;
        SectionTracker sections;
    };

} // namespace SectionTracking

using SectionTracking::TestCaseTracker;

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_TEST_CASE_TRACKER_HPP_INCLUDED
