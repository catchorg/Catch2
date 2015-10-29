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
    struct ITracker : SharedImpl<> {
        virtual ~ITracker() {}
        
        // static queries
        virtual std::string name() const = 0;
        
        // dynamic queries
        virtual bool isComplete() const = 0; // Successfully completed or failed
        virtual bool isSuccessfullyCompleted() const = 0;
        virtual bool isOpen() const = 0; // Started but not complete
        
        virtual ITracker& parent() = 0;
        
        // actions
        virtual void close() = 0; // Successfully complete
        virtual void fail() = 0;
        virtual void markAsNeedingAnotherRun() = 0;

        virtual void addChild( Ptr<ITracker> const& child ) = 0;
        virtual ITracker* findChild( std::string const& name ) = 0;
        virtual void openChild() = 0;
    };
    

    class TrackerContext {
        
        enum RunState {
            NotStarted,
            Executing,
            CompletedCycle
        };
        
        Ptr<ITracker> m_rootTracker;
        ITracker* m_currentTracker;
        RunState m_runState;
        
    public:
        
        static TrackerContext& instance() {
            static TrackerContext s_instance;
            return s_instance;
        }

        TrackerContext()
        :   m_currentTracker( CATCH_NULL ),
            m_runState( NotStarted )
        {}


        ITracker& startRun();
        
        void endRun() {
            m_rootTracker.reset();
            m_currentTracker = CATCH_NULL;
            m_runState = NotStarted;
        }

        void startCycle() {
            m_currentTracker = m_rootTracker.get();
            m_runState = Executing;
        }
        void completeCycle() {
            m_runState = CompletedCycle;
        }
        
        bool completedCycle() const {
            return m_runState == CompletedCycle;
        }
        
        ITracker& currentTracker() {
            return *m_currentTracker;
        }
        void setCurrentTracker( ITracker* tracker ) {
            m_currentTracker = tracker;
        }
    };
    
    class TrackerBase : public ITracker {
    protected:
        enum CycleState {
            NotStarted,
            Executing,
            ExecutingChildren,
            NeedsAnotherRun,
            CompletedSuccessfully,
            Failed
        };
        class TrackerHasName {
            std::string m_name;
        public:
            TrackerHasName( std::string const& name ) : m_name( name ) {}
            bool operator ()( Ptr<ITracker> const& tracker ) {
                return tracker->name() == m_name;
            }
        };
        typedef std::vector<Ptr<ITracker> > Children;
        std::string m_name;
        TrackerContext& m_ctx;
        ITracker* m_parent;
        Children m_children;
        CycleState m_runState;
    public:
        TrackerBase( std::string const& name, TrackerContext& ctx, ITracker* parent )
        :   m_name( name ),
            m_ctx( ctx ),
            m_parent( parent ),
            m_runState( NotStarted )
        {}
        
        virtual std::string name() const CATCH_OVERRIDE {
            return m_name;
        }
        virtual bool isComplete() const CATCH_OVERRIDE {
            return m_runState == CompletedSuccessfully || m_runState == Failed;
        }
        virtual bool isSuccessfullyCompleted() const CATCH_OVERRIDE {
            return m_runState == CompletedSuccessfully;
        }
        virtual bool isOpen() const CATCH_OVERRIDE {
            return m_runState != NotStarted && !isComplete();
        }
        
        
        virtual void addChild( Ptr<ITracker> const& child ) CATCH_OVERRIDE {
            m_children.push_back( child );
            size_t childCount = m_children.size();
        }
        
        virtual ITracker* findChild( std::string const& name ) CATCH_OVERRIDE {
            Children::const_iterator it = std::find_if( m_children.begin(), m_children.end(), TrackerHasName( name ) );
            return( it != m_children.end() )
                ? it->get()
                : CATCH_NULL;
        }
        virtual ITracker& parent() CATCH_OVERRIDE {
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
        void open() {
            m_runState = Executing;
            moveToThis();
            if( m_parent )
                m_parent->openChild();
        }
        
        virtual void close() CATCH_OVERRIDE {
            
            // Close any still open children (e.g. generators)
            while( &m_ctx.currentTracker() != this )
                m_ctx.currentTracker().close();

            switch( m_runState ) {
                case CompletedSuccessfully:
                case Failed:
                    throw std::logic_error( "Illogical state" );

                case NeedsAnotherRun:
                    break;;

                case Executing:
                    m_runState = CompletedSuccessfully;
                    break;
                case ExecutingChildren:
                    if( m_children.empty() || m_children.back()->isComplete() )
                        m_runState = CompletedSuccessfully;
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
                m_parent->markAsNeedingAnotherRun();
            moveToParent();
            m_ctx.completeCycle();
        }
        virtual void markAsNeedingAnotherRun() CATCH_OVERRIDE {
            m_runState = NeedsAnotherRun;
        }
    private:
        void moveToParent() {
            assert( m_parent );
            m_ctx.setCurrentTracker( m_parent );
        }
        void moveToThis() {
            m_ctx.setCurrentTracker( this );
        }
    };
    

    class SectionTracker : public TrackerBase {
    public:
        SectionTracker( std::string const& name, TrackerContext& ctx, ITracker* parent )
        :   TrackerBase( name, ctx, parent )
        {}
        
        static SectionTracker& acquire( TrackerContext& ctx, std::string const& name ) {
            SectionTracker* section = CATCH_NULL;
            
            ITracker& currentTracker = ctx.currentTracker();
            if( ITracker* childTracker = currentTracker.findChild( name ) ) {
                section = dynamic_cast<SectionTracker*>( childTracker );
                assert( section );
            }
            else {
                section = new SectionTracker( name, ctx, &currentTracker );
                currentTracker.addChild( section );
            }
            if( !ctx.completedCycle() && !section->isComplete() ) {
                
                section->open();
            }
            return *section;
        }
    };
    
    class IndexTracker : public TrackerBase {
        int m_size;
        int m_index;
    public:
        IndexTracker( std::string const& name, TrackerContext& ctx, ITracker* parent, int size )
        :   TrackerBase( name, ctx, parent ),
            m_size( size ),
            m_index( -1 )
        {}
        
        static IndexTracker& acquire( TrackerContext& ctx, std::string const& name, int size ) {
            IndexTracker* tracker = CATCH_NULL;
            
            ITracker& currentTracker = ctx.currentTracker();
            if( ITracker* childTracker = currentTracker.findChild( name ) ) {
                tracker = dynamic_cast<IndexTracker*>( childTracker );
                assert( tracker );
            }
            else {
                tracker = new IndexTracker( name, ctx, &currentTracker, size );
                currentTracker.addChild( tracker );
            }
            
            if( !ctx.completedCycle() && !tracker->isComplete() ) {
                if( tracker->m_runState != ExecutingChildren && tracker->m_runState != NeedsAnotherRun )
                    tracker->moveNext();
                tracker->open();
            }
            
            return *tracker;
        }
        
        int index() const { return m_index; }
        
        void moveNext() {
            m_index++;
            m_children.clear();
        }
        
        virtual void close() CATCH_OVERRIDE {
            TrackerBase::close();
            if( m_runState == CompletedSuccessfully )
                if( m_index < m_size-1 )
                    m_runState = Executing;
        }
    };
    
    ITracker& TrackerContext::startRun() {
        m_rootTracker = new SectionTracker( "{root}", *this, CATCH_NULL );
        m_currentTracker = CATCH_NULL;
        m_runState = Executing;
        return *m_rootTracker;
    }
    
    class LocalContext {
        
    public:
        TrackerContext& operator()() const {
            return TrackerContext::instance();
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

//    REQUIRE( C_A_T_C_H_Context().i() == 42 );
}

TEST_CASE( "Tracker" ) {
    
    TrackerContext ctx;
    ctx.startRun();
    ctx.startCycle();
    
    ITracker& testCase = SectionTracker::acquire( ctx, "Testcase" );
    REQUIRE( testCase.isOpen() );

    ITracker& s1 = SectionTracker::acquire( ctx, "S1" );
    REQUIRE( s1.isOpen() );

    SECTION( "successfully close one section" ) {
        s1.close();
        REQUIRE( s1.isSuccessfullyCompleted() );
        REQUIRE( testCase.isComplete() == false );

        testCase.close();
        REQUIRE( ctx.completedCycle() );
        REQUIRE( testCase.isSuccessfullyCompleted() );
    }
    
    SECTION( "fail one section" ) {
        s1.fail();
        REQUIRE( s1.isComplete() );
        REQUIRE( s1.isSuccessfullyCompleted() == false );
        REQUIRE( testCase.isComplete() == false );
        
        testCase.close();
        REQUIRE( ctx.completedCycle() );
        REQUIRE( testCase.isSuccessfullyCompleted() == false );

        SECTION( "re-enter after failed section" ) {
            ctx.startCycle();
            ITracker& testCase2 = SectionTracker::acquire( ctx, "Testcase" );
            REQUIRE( testCase2.isOpen() );
            
            ITracker& s1b = SectionTracker::acquire( ctx, "S1" );
            REQUIRE( s1b.isOpen() == false );
            
            testCase2.close();
            REQUIRE( ctx.completedCycle() );
            REQUIRE( testCase.isComplete() );
            REQUIRE( testCase.isSuccessfullyCompleted() );
        }
        SECTION( "re-enter after failed section and find next section" ) {
            ctx.startCycle();
            ITracker& testCase2 = SectionTracker::acquire( ctx, "Testcase" );
            REQUIRE( testCase2.isOpen() );
            
            ITracker& s1b = SectionTracker::acquire( ctx, "S1" );
            REQUIRE( s1b.isOpen() == false );

            ITracker& s2 = SectionTracker::acquire( ctx, "S2" );
            REQUIRE( s2.isOpen() );

            s2.close();
            REQUIRE( ctx.completedCycle() );
            
            testCase2.close();
            REQUIRE( testCase.isComplete() );
            REQUIRE( testCase.isSuccessfullyCompleted() );
        }
    }
    
    SECTION( "successfully close one section, then find another" ) {
        s1.close();
        
        ITracker& s2 = SectionTracker::acquire( ctx, "S2" );
        REQUIRE( s2.isOpen() == false );
        
        testCase.close();
        REQUIRE( testCase.isComplete() == false );

        SECTION( "Re-enter - skips S1 and enters S2" ) {
            ctx.startCycle();
            ITracker& testCase2 = SectionTracker::acquire( ctx, "Testcase" );
            REQUIRE( testCase2.isOpen() );
            
            ITracker& s1b = SectionTracker::acquire( ctx, "S1" );
            REQUIRE( s1b.isOpen() == false );

            ITracker& s2b = SectionTracker::acquire( ctx, "S2" );
            REQUIRE( s2b.isOpen() );

            REQUIRE( ctx.completedCycle() == false );
            
            SECTION ("Successfully close S2") {
                s2b.close();
                REQUIRE( ctx.completedCycle() );

                REQUIRE( s2b.isSuccessfullyCompleted() );
                REQUIRE( testCase2.isComplete() == false );
                
                testCase2.close();
                REQUIRE( testCase2.isSuccessfullyCompleted() );
            }
            SECTION ("fail S2") {
                s2b.fail();
                REQUIRE( ctx.completedCycle() );

                REQUIRE( s2b.isComplete() );
                REQUIRE( s2b.isSuccessfullyCompleted() == false );
                
                testCase2.close();
                REQUIRE( testCase2.isSuccessfullyCompleted() == false );

                // Need a final cycle
                ctx.startCycle();
                ITracker& testCase3 = SectionTracker::acquire( ctx, "Testcase" );
                REQUIRE( testCase3.isOpen() );
                
                ITracker& s1c = SectionTracker::acquire( ctx, "S1" );
                REQUIRE( s1c.isOpen() == false );
                
                ITracker& s2c = SectionTracker::acquire( ctx, "S2" );
                REQUIRE( s2c.isOpen() == false );
            
                testCase3.close();
                REQUIRE( testCase3.isSuccessfullyCompleted() );
            }
        }
    }
    
    SECTION( "open a nested section" ) {
        ITracker& s2 = SectionTracker::acquire( ctx, "S2" );
        REQUIRE( s2.isOpen() );

        s2.close();
        REQUIRE( s2.isComplete() );
        REQUIRE( s1.isComplete() == false );
        
        s1.close();
        REQUIRE( s1.isComplete() );
        REQUIRE( testCase.isComplete() == false );
        
        testCase.close();
        REQUIRE( testCase.isComplete() );
    }
    
    SECTION( "start a generator" ) {
        IndexTracker& g1 = IndexTracker::acquire( ctx, "G1", 2 );
        REQUIRE( g1.isOpen() );
        REQUIRE( g1.index() == 0 );

        REQUIRE( g1.isComplete() == false );
        REQUIRE( s1.isComplete() == false );

        SECTION( "close outer section" )
        {
            s1.close();
            REQUIRE( s1.isComplete() == false );
            testCase.close();
            REQUIRE( testCase.isSuccessfullyCompleted() == false );

            SECTION( "Re-enter for second generation" ) {
                ctx.startCycle();
                ITracker& testCase2 = SectionTracker::acquire( ctx, "Testcase" );
                REQUIRE( testCase2.isOpen() );
                
                ITracker& s1b = SectionTracker::acquire( ctx, "S1" );
                REQUIRE( s1b.isOpen() );
                
                
                IndexTracker& g1b = IndexTracker::acquire( ctx, "G1", 2 );
                REQUIRE( g1b.isOpen() );
                REQUIRE( g1b.index() == 1 );
                
                REQUIRE( s1.isComplete() == false );
                
                s1b.close();
                REQUIRE( s1b.isComplete() );
                REQUIRE( g1b.isComplete() );
                testCase2.close();
                REQUIRE( testCase2.isComplete() );
            }
        }
        SECTION( "Start a new inner section" ) {
            ITracker& s2 = SectionTracker::acquire( ctx, "S2" );
            REQUIRE( s2.isOpen() );

            s2.close();
            REQUIRE( s2.isComplete() );

            s1.close();
            REQUIRE( s1.isComplete() == false );

            testCase.close();
            REQUIRE( testCase.isComplete() == false );
            
            SECTION( "Re-enter for second generation" ) {
                ctx.startCycle();
                ITracker& testCase2 = SectionTracker::acquire( ctx, "Testcase" );
                REQUIRE( testCase2.isOpen() );
                
                ITracker& s1b = SectionTracker::acquire( ctx, "S1" );
                REQUIRE( s1b.isOpen() );
                
                // generator - next value
                IndexTracker& g1b = IndexTracker::acquire( ctx, "G1", 2 );
                REQUIRE( g1b.isOpen() );
                REQUIRE( g1b.index() == 1 );
                
                // inner section again
                ITracker& s2b = SectionTracker::acquire( ctx, "S2" );
                REQUIRE( s2b.isOpen() );
                
                s2b.close();
                REQUIRE( s2b.isComplete() );
                
                s1b.close();
                REQUIRE( g1b.isComplete() );
                REQUIRE( s1b.isComplete() );
                
                testCase2.close();
                REQUIRE( testCase2.isComplete() );
            }
        }
        
        SECTION( "Fail an inner section" ) {
            ITracker& s2 = SectionTracker::acquire( ctx, "S2" );
            REQUIRE( s2.isOpen() );
            
            s2.fail();
            REQUIRE( s2.isComplete() );
            REQUIRE( s2.isSuccessfullyCompleted() == false );
            
            s1.close();
            REQUIRE( s1.isComplete() == false );
            
            testCase.close();
            REQUIRE( testCase.isComplete() == false );
            
            SECTION( "Re-enter for second generation" ) {
                ctx.startCycle();
                ITracker& testCase2 = SectionTracker::acquire( ctx, "Testcase" );
                REQUIRE( testCase2.isOpen() );
                
                ITracker& s1b = SectionTracker::acquire( ctx, "S1" );
                REQUIRE( s1b.isOpen() );
                
                // generator - still same value
                IndexTracker& g1b = IndexTracker::acquire( ctx, "G1", 2 );
                REQUIRE( g1b.isOpen() );
                REQUIRE( g1b.index() == 0 );
                
                // inner section again - this time won't open
                ITracker& s2b = SectionTracker::acquire( ctx, "S2" );
                REQUIRE( s2b.isOpen() == false );
                
                s1b.close();
                REQUIRE( g1b.isComplete() == false );
                REQUIRE( s1b.isComplete() == false );
                
                testCase2.close();
                REQUIRE( testCase2.isComplete() == false );
                
                // Another cycle - now should complete
                ctx.startCycle();
                ITracker& testCase3 = SectionTracker::acquire( ctx, "Testcase" );
                REQUIRE( testCase3.isOpen() );
                
                ITracker& s1c = SectionTracker::acquire( ctx, "S1" );
                REQUIRE( s1c.isOpen() );
                
                // generator - now next value
                IndexTracker& g1c = IndexTracker::acquire( ctx, "G1", 2 );
                REQUIRE( g1c.isOpen() );
                REQUIRE( g1c.index() == 1 );
                
                // inner section - now should open again
                ITracker& s2c = SectionTracker::acquire( ctx, "S2" );
                REQUIRE( s2c.isOpen() );
                
                s2c.close();
                REQUIRE( s2c.isComplete() );
                
                s1c.close();
                REQUIRE( g1c.isComplete() );
                REQUIRE( s1c.isComplete() );
                
                testCase3.close();
                REQUIRE( testCase3.isComplete() );
            }
        }
        // !TBD"
        //   nested generator
        //   two sections within a generator
    }
}
