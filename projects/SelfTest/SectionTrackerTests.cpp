/*
 *  Created by Phil on 20/07/2013.
 *  Copyright 2013 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch.hpp"

using namespace Catch;

class TrackedSection;

typedef std::map<std::string, TrackedSection> TrackedSections;

class TrackedSection {
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
    SectionTracker( std::string const& testCaseName )
    :   m_testCase( testCaseName, NULL ),
        m_currentSection( NULL ),
        m_completedASectionThisRun( false )
    {}
    
    void enter() {
        m_completedASectionThisRun = false;
        m_currentSection = &m_testCase;
        m_testCase.enter();
    }
    void leave() {
        m_testCase.leave();
    }

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


    bool isCompleted() const {
        return m_testCase.runState() == TrackedSection::Completed;
    }
    bool hasCompletedASectionThisRun() const {
        return m_completedASectionThisRun;
    }

private:
    TrackedSection m_testCase;
    TrackedSection* m_currentSection;
    bool m_completedASectionThisRun;
};

TEST_CASE( "sections" ) {

    using namespace Catch;
    SectionTracker sectionTracker( "test case" );

    const std::string section1Name = "section 1";
    const std::string section2Name = "section 2";

    CHECK_FALSE( sectionTracker.isCompleted() );

    SECTION( "test case with no sections" ) {

        sectionTracker.enter();
        CHECK_FALSE( sectionTracker.isCompleted() );

        sectionTracker.leave();
        CHECK( sectionTracker.isCompleted() );
    }

    SECTION( "test case with one section" ) {

        // Enter test case
        sectionTracker.enter();

        // Enter section? - no, not yet
        CHECK_FALSE( sectionTracker.enterSection( section1Name ) );
        CHECK_FALSE( sectionTracker.isCompleted() );

        // Leave test case - incomplete (still need to visit section)
        sectionTracker.leave();
        CHECK_FALSE( sectionTracker.isCompleted() );

        // ...

        // Enter test case again
        sectionTracker.enter();

        // Enter section? - yes
        CHECK( sectionTracker.enterSection( section1Name ) );

        // Leave section and test case - now complete
        sectionTracker.leaveSection();
        sectionTracker.leave();
        CHECK( sectionTracker.isCompleted() );
    }

    SECTION( "test case with two consecutive sections" ) {

        // Enter test case
        sectionTracker.enter();

        // Enter section 1? - no, not yet
        CHECK_FALSE( sectionTracker.enterSection( section1Name ) );

        // Enter section 2? - no, not yet
        CHECK_FALSE( sectionTracker.enterSection( section2Name ) );

        // Leave test case - incomplete (still need to visit sections)
        sectionTracker.leave();
        CHECK_FALSE( sectionTracker.isCompleted() );

        // ...

        // Enter test case again
        sectionTracker.enter();

        // Enter section 1? - yes
        CHECK( sectionTracker.enterSection( section1Name ) );
        sectionTracker.leaveSection();

        // Enter section 2? - no, not yet
        CHECK_FALSE( sectionTracker.enterSection( section2Name ) );

        // Leave test case - incomplete (still need to visit section 2)
        sectionTracker.leave();
        CHECK_FALSE( sectionTracker.isCompleted() );

        // ...

        // Enter test case again
        sectionTracker.enter();

        // Enter section 1? - no, already done now
        CHECK_FALSE( sectionTracker.enterSection( section1Name ) );

        // Enter section 2? - yes - finally
        CHECK( sectionTracker.enterSection( section2Name ) );
        sectionTracker.leaveSection();

        // Leave test case - now complete
        sectionTracker.leave();
        CHECK( sectionTracker.isCompleted() );        
    }

    SECTION( "test case with one section within another" ) {

        // Enter test case
        sectionTracker.enter();

        // Enter section 1? - no, not yet
        CHECK_FALSE( sectionTracker.enterSection( section1Name ) );

        // Leave test case - incomplete (still need to visit sections)
        sectionTracker.leave();
        CHECK_FALSE( sectionTracker.isCompleted() );

        // ...

        // Enter test case again
        sectionTracker.enter();

        // Enter section 1? - yes
        CHECK( sectionTracker.enterSection( section1Name ) );

        // Enter section 2? - no, not yet
        CHECK_FALSE( sectionTracker.enterSection( section2Name ) );

        sectionTracker.leaveSection(); // section 1 - incomplete (section 2)

        // Leave test case - incomplete
        sectionTracker.leave();
        CHECK_FALSE( sectionTracker.isCompleted() );

        // ...

        // Enter test case again
        sectionTracker.enter();

        // Enter section 1? - yes - so we can execute section 2
        CHECK( sectionTracker.enterSection( section1Name ) );

        // Enter section 2? - yes - finally
        CHECK( sectionTracker.enterSection( section2Name ) );
        sectionTracker.leaveSection(); // section 2
        sectionTracker.leaveSection(); // section 1

        // Leave test case - now complete
        sectionTracker.leave();
        CHECK( sectionTracker.isCompleted() );        
    }
}
