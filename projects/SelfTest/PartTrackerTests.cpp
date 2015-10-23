/*
 *  Created by Phil on 1/10/2015.
 *  Copyright 2015 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "internal/catch_suppress_warnings.h"
#include "internal/catch_compiler_capabilities.h"
#include "internal/catch_ptr.hpp"

#ifdef __clang__
//#   pragma clang diagnostic ignored "-Wpadded"
//#   pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#include <cassert>
#include <memory>
#include <vector>

namespace Catch
{
    struct IContainerPart;
    
    struct ITrackedPart : SharedImpl<> {
        virtual ~ITrackedPart() {}
        virtual std::string name() const = 0;
        virtual bool isUnstarted() const = 0;
        virtual bool isCompleteOrFailed() const = 0;
        virtual bool isComplete() const = 0;
        virtual bool isOpen() const = 0;
        
        virtual IContainerPart& parent() = 0;
        
        virtual void close() = 0;
        virtual void fail() = 0;
    };
    
    struct IContainerPart : ITrackedPart {
        virtual void addChild( Ptr<ITrackedPart> const& child ) = 0;
        virtual ITrackedPart* findChild( std::string const& name ) = 0;
        virtual void openChild() = 0;
        virtual void childFailed() = 0;
    };
 

    class TrackerContext {
        
        enum RunState {
            NotStarted,
            Executing,
            CompletedCycle
        };
        
        Ptr<IContainerPart> m_rootPartTracker;
        IContainerPart* m_currentPart;
        RunState m_runState;
        
    public:
        
        static TrackerContext& instance() {
            static TrackerContext s_instance;
            return s_instance;
        }

        TrackerContext()
        :   m_currentPart( CATCH_NULL ),
            m_runState( NotStarted )
        {}
        
        int i() {
            return 42;
        }

        IContainerPart& startRun();
        
        void endRun() {
            m_rootPartTracker.reset();
            m_currentPart = CATCH_NULL;
            m_runState = NotStarted;
        }

        void startCycle() {
            m_currentPart = m_rootPartTracker.get();
            m_runState = Executing;
        }
        void completeCycle() {
            m_runState = CompletedCycle;
        }
        
        bool completedCycle() const {
            return m_runState == CompletedCycle;
        }
        
        IContainerPart& currentPart() {
            return *m_currentPart;
        }
        void setCurrentPart( IContainerPart* part ) {
            m_currentPart = part;
        }
        
        ITrackedPart* findPart( std::string const& name ) {
            return m_currentPart->findChild( name );
        }
        

    private:
    };

    
    class PartTracker : public IContainerPart {
        enum RunState {
            NotStarted,
            Executing,
            ExecutingChildren,
            CompletedSuccessfully,
            Failed,
            ChildFailed
        };
        class TrackerHasName {
            std::string m_name;
        public:
            TrackerHasName( std::string const& name ) : m_name( name ) {}
            bool operator ()( Ptr<ITrackedPart> const& tracker ) {
                return tracker->name() == m_name;
            }
        };
        typedef std::vector<Ptr<ITrackedPart> > Children;
        std::string m_name;
        TrackerContext& m_ctx;
        IContainerPart* m_parent;
        Children m_children;
        RunState m_runState;
    public:
        PartTracker( std::string const& name, TrackerContext& ctx, IContainerPart* parent )
        :   m_name( name ),
            m_ctx( ctx ),
            m_parent( parent ),
            m_runState( NotStarted ) {}
        
        virtual std::string name() const CATCH_OVERRIDE {
            return m_name;
        }
        
        virtual bool isCompleteOrFailed() const CATCH_OVERRIDE {
            return m_runState == CompletedSuccessfully || m_runState == Failed;
        }
        virtual bool isComplete() const CATCH_OVERRIDE {
            return m_runState == CompletedSuccessfully;
        }
        virtual bool isUnstarted() const CATCH_OVERRIDE {
            return m_runState == NotStarted;
        }
        virtual bool isOpen() const CATCH_OVERRIDE {
            return m_runState == Executing || m_runState == ExecutingChildren;
        }
        
        
        virtual void addChild( Ptr<ITrackedPart> const& child ) CATCH_OVERRIDE {
            m_children.push_back( child );
            size_t childCount = m_children.size();
        }
        
        virtual ITrackedPart* findChild( std::string const& name ) CATCH_OVERRIDE {
            Children::const_iterator it = std::find_if( m_children.begin(), m_children.end(), TrackerHasName( name ) );
            return( it != m_children.end() )
            ? it->get()
            : CATCH_NULL;
        }
        virtual IContainerPart& parent() CATCH_OVERRIDE {
            assert( m_parent ); // Should always be non-null except for root
            return *m_parent;
        }
        
        virtual void openChild() CATCH_OVERRIDE {
            if( m_runState != ExecutingChildren ) {
                m_runState = ExecutingChildren;
                if( m_parent )
                    m_parent->openChild();
            }
        }
        virtual void childFailed() CATCH_OVERRIDE {
            assert( m_runState == ExecutingChildren );
            m_runState = ChildFailed;
            if( m_parent )
                m_parent->childFailed();
        }
        void open() {
            m_runState = Executing;
            moveToThis();
            if( m_parent )
                m_parent->openChild();
        }
        
        virtual void close() CATCH_OVERRIDE {
            switch( m_runState ) {
                case Executing:
                    m_runState = CompletedSuccessfully;
                    break;
                case ExecutingChildren:
                    if( !hasUnstartedChildren() )
                        m_runState = CompletedSuccessfully;
                    break;
                case ChildFailed:
                    m_runState = ExecutingChildren;
                    break;
                default:
                    throw std::logic_error( "Unexpected state" );
            }
            moveToParent();
            m_ctx.completeCycle();
        }
        virtual void fail() CATCH_OVERRIDE {
            m_runState = Failed;
            if( m_parent )
                m_parent->childFailed();
            moveToParent();
            m_ctx.completeCycle();
        }
    private:
        void moveToParent() {
            m_ctx.setCurrentPart( m_parent );
        }
        void moveToThis() {
            m_ctx.setCurrentPart( this );
        }

        bool hasUnstartedChildren() const {
            return !m_children.empty() && m_children.back()->isUnstarted();
        }
    };
    
    IContainerPart& TrackerContext::startRun() {
        m_rootPartTracker = new PartTracker( "{root}", *this, CATCH_NULL );
        m_currentPart = CATCH_NULL;
        m_runState = Executing;
        return *m_rootPartTracker;
    }

    class LocalContext {
    
    public:
        TrackerContext& operator()() const {
            return TrackerContext::instance();
        }
    };

    class SectionPart : public PartTracker {
    public:
        SectionPart( std::string const& name, TrackerContext& ctx, IContainerPart* parent )
        :   PartTracker( name, ctx, parent )
        {}
        
        static SectionPart& acquire( TrackerContext& ctx, std::string const& name ) {
            SectionPart* section = CATCH_NULL;
            
            IContainerPart& currentPart = ctx.currentPart();
            if( ITrackedPart* part = currentPart.findChild( name ) ) {
                section = dynamic_cast<SectionPart*>( part );
                assert( section );
            }
            else {
                section = new SectionPart( name, ctx, &currentPart );
                currentPart.addChild( section );
            }
            if( !ctx.completedCycle() && !section->isCompleteOrFailed() ) {
                section->open();
            }
            return *section;
        }
    };
    
} // namespace Catch

inline Catch::TrackerContext& C_A_T_C_H_Context() {
    return Catch::TrackerContext::instance();
}

// -------------------

#include "catch.hpp"

using namespace Catch;

inline void testCase( Catch::LocalContext const& C_A_T_C_H_Context ) {

    REQUIRE( C_A_T_C_H_Context().i() == 42 );
}

TEST_CASE( "PartTracker" ) {
    
    TrackerContext ctx;
    ctx.startRun();
    ctx.startCycle();
    
    SectionPart& testCase = SectionPart::acquire( ctx, "Testcase" );
    REQUIRE( testCase.isComplete() == false );

    SectionPart& s1 = SectionPart::acquire( ctx, "S1" );
    REQUIRE( s1.isComplete() == false );

    SECTION( "successfully close one section" ) {
        s1.close();
        REQUIRE( s1.isComplete() == true );
        REQUIRE( testCase.isCompleteOrFailed() == false );

        testCase.close();
        REQUIRE( testCase.isComplete() == true );
        
        REQUIRE( ctx.completedCycle() == true );
    }
    
    SECTION( "fail one section" ) {
        s1.fail();
        REQUIRE( s1.isComplete() == false );
        REQUIRE( s1.isCompleteOrFailed() == true );
        REQUIRE( testCase.isComplete() == false );
        REQUIRE( testCase.isCompleteOrFailed() == false );
        
        testCase.close();
        REQUIRE( ctx.completedCycle() == true );
        REQUIRE( testCase.isComplete() == false );

        SECTION( "re-enter after failed section" ) {
            ctx.startCycle();
            SectionPart& testCase2 = SectionPart::acquire( ctx, "Testcase" );
            REQUIRE( testCase2.isComplete() == false );
            
            SectionPart& s1b = SectionPart::acquire( ctx, "S1" );
            REQUIRE( s1b.isComplete() == false );

            testCase2.close();
            REQUIRE( ctx.completedCycle() == true );
            REQUIRE( testCase.isComplete() == true );
            REQUIRE( testCase.isCompleteOrFailed() == true );
        }
        SECTION( "re-enter after failed section and find next section" ) {
            ctx.startCycle();
            SectionPart& testCase2 = SectionPart::acquire( ctx, "Testcase" );
            REQUIRE( testCase2.isComplete() == false );
            
            SectionPart& s1b = SectionPart::acquire( ctx, "S1" );
            REQUIRE( s1b.isComplete() == false );

            SectionPart& s2 = SectionPart::acquire( ctx, "S2" );
            REQUIRE( s2.isOpen() );
            s2.close();
            REQUIRE( ctx.completedCycle() == true );
            
            testCase2.close();
            REQUIRE( testCase.isComplete() == true );
            REQUIRE( testCase.isCompleteOrFailed() == true );
        }
    }
    
    SECTION( "successfully close one section, then find another" ) {
        s1.close();
        REQUIRE( ctx.completedCycle() == true );
        
        SectionPart& s2 = SectionPart::acquire( ctx, "S2" );
        REQUIRE( s2.isComplete() == false );
        REQUIRE( s2.isOpen() == false );
        
        testCase.close();
        REQUIRE( testCase.isComplete() == false );

        SECTION( "Re-enter - skip S1 and enter S2" ) {
            ctx.startCycle();
            SectionPart& testCase2 = SectionPart::acquire( ctx, "Testcase" );
            REQUIRE( testCase2.isComplete() == false );
            
            SectionPart& s1b = SectionPart::acquire( ctx, "S1" );
            REQUIRE( s1b.isComplete() == true );

            SectionPart& s2b = SectionPart::acquire( ctx, "S2" );
            REQUIRE( s2b.isComplete() == false );
            REQUIRE( s2b.isOpen() );

            REQUIRE( ctx.completedCycle() == false );
            
            SECTION ("Successfully close S2") {
                s2b.close();
                REQUIRE( ctx.completedCycle() == true );

                REQUIRE( s2b.isComplete() == true );
                REQUIRE( testCase2.isCompleteOrFailed() == false );
                
                testCase2.close();
                REQUIRE( testCase2.isComplete() == true );
            }
            SECTION ("fail S2") {
                s2b.fail();
                REQUIRE( ctx.completedCycle() == true );

                REQUIRE( s2b.isComplete() == false );
                REQUIRE( s2b.isCompleteOrFailed() == true );
                REQUIRE( testCase2.isCompleteOrFailed() == false );
                
                testCase2.close();
                REQUIRE( testCase2.isComplete() == false );
            }
        }
    }
    
    SECTION( "open a nested section" ) {
        SectionPart& s2 = SectionPart::acquire( ctx, "S2" );
        REQUIRE( s2.isOpen() == true );

        s2.close();
        REQUIRE( s2.isComplete() == true );
        REQUIRE( s1.isComplete() == false );
        
        s1.close();
        REQUIRE( s1.isComplete() == true );
        REQUIRE( testCase.isComplete() == false );
        
        testCase.close();
        REQUIRE( testCase.isComplete() == true );
        
    }
    
}
