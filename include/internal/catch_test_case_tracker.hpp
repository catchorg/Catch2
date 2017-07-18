/*
 *  Created by Phil Nash on 23/7/2013
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_TRACKER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_TRACKER_HPP_INCLUDED

#include "catch_compiler_capabilities.h"
#include "catch_common.h"

#include <algorithm>
#include <string>
#include <assert.h>
#include <vector>
#include <stdexcept>
#include <memory>

CATCH_INTERNAL_SUPPRESS_ETD_WARNINGS

namespace Catch {
namespace TestCaseTracking {

    struct NameAndLocation {
        std::string name;
        SourceLineInfo location;

        NameAndLocation( std::string const& _name, SourceLineInfo const& _location )
        :   name( _name ),
            location( _location )
        {}
    };

    struct ITracker;

    using ITrackerPtr = std::shared_ptr<ITracker>;

    struct ITracker {
        virtual ~ITracker();

        // static queries
        virtual NameAndLocation const& nameAndLocation() const = 0;

        // dynamic queries
        virtual bool isComplete() const = 0; // Successfully completed or failed
        virtual bool isSuccessfullyCompleted() const = 0;
        virtual bool isOpen() const = 0; // Started but not complete
        virtual bool hasChildren() const = 0;

        virtual ITracker& parent() = 0;

        // actions
        virtual void close() = 0; // Successfully complete
        virtual void fail() = 0;
        virtual void markAsNeedingAnotherRun() = 0;

        virtual void addChild( ITrackerPtr const& child ) = 0;
        virtual ITrackerPtr findChild( NameAndLocation const& nameAndLocation ) = 0;
        virtual void openChild() = 0;

        // Debug/ checking
        virtual bool isSectionTracker() const = 0;
        virtual bool isIndexTracker() const = 0;
    };

    class  TrackerContext {

        enum RunState {
            NotStarted,
            Executing,
            CompletedCycle
        };

        ITrackerPtr m_rootTracker;
        ITracker* m_currentTracker = nullptr;
        RunState m_runState = NotStarted;

    public:

        static TrackerContext& instance() {
            static TrackerContext s_instance;
            return s_instance;
        }

        ITracker& startRun();

        void endRun() {
            m_rootTracker.reset();
            m_currentTracker = nullptr;
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
            NameAndLocation m_nameAndLocation;
        public:
            TrackerHasName( NameAndLocation const& nameAndLocation ) : m_nameAndLocation( nameAndLocation ) {}
            bool operator ()( ITrackerPtr const& tracker ) {
                return
                    tracker->nameAndLocation().name == m_nameAndLocation.name &&
                    tracker->nameAndLocation().location == m_nameAndLocation.location;
            }
        };
        typedef std::vector<ITrackerPtr> Children;
        NameAndLocation m_nameAndLocation;
        TrackerContext& m_ctx;
        ITracker* m_parent;
        Children m_children;
        CycleState m_runState = NotStarted;
    public:
        TrackerBase( NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent )
        :   m_nameAndLocation( nameAndLocation ),
            m_ctx( ctx ),
            m_parent( parent )
        {}
        virtual ~TrackerBase();

        virtual NameAndLocation const& nameAndLocation() const override {
            return m_nameAndLocation;
        }
        virtual bool isComplete() const override {
            return m_runState == CompletedSuccessfully || m_runState == Failed;
        }
        virtual bool isSuccessfullyCompleted() const override {
            return m_runState == CompletedSuccessfully;
        }
        virtual bool isOpen() const override {
            return m_runState != NotStarted && !isComplete();
        }
        virtual bool hasChildren() const override {
            return !m_children.empty();
        }


        virtual void addChild( ITrackerPtr const& child ) override {
            m_children.push_back( child );
        }

        virtual ITrackerPtr findChild( NameAndLocation const& nameAndLocation ) override {
            auto it = std::find_if( m_children.begin(), m_children.end(), TrackerHasName( nameAndLocation ) );
            return( it != m_children.end() )
                ? *it
                : nullptr;
        }
        virtual ITracker& parent() override {
            assert( m_parent ); // Should always be non-null except for root
            return *m_parent;
        }

        virtual void openChild() override {
            if( m_runState != ExecutingChildren ) {
                m_runState = ExecutingChildren;
                if( m_parent )
                    m_parent->openChild();
            }
        }

        virtual bool isSectionTracker() const override { return false; }
        virtual bool isIndexTracker() const override { return false; }

        void open() {
            m_runState = Executing;
            moveToThis();
            if( m_parent )
                m_parent->openChild();
        }

        virtual void close() override {

            // Close any still open children (e.g. generators)
            while( &m_ctx.currentTracker() != this )
                m_ctx.currentTracker().close();

            switch( m_runState ) {
                case NeedsAnotherRun:
                    break;;

                case Executing:
                    m_runState = CompletedSuccessfully;
                    break;
                case ExecutingChildren:
                    if( m_children.empty() || m_children.back()->isComplete() )
                        m_runState = CompletedSuccessfully;
                    break;

                case NotStarted:
                case CompletedSuccessfully:
                case Failed:
                    CATCH_INTERNAL_ERROR( "Illogical state: " << m_runState );

                default:
                    CATCH_INTERNAL_ERROR( "Unknown state: " << m_runState );
            }
            moveToParent();
            m_ctx.completeCycle();
        }
        virtual void fail() override {
            m_runState = Failed;
            if( m_parent )
                m_parent->markAsNeedingAnotherRun();
            moveToParent();
            m_ctx.completeCycle();
        }
        virtual void markAsNeedingAnotherRun() override {
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
        std::vector<std::string> m_filters;
    public:
        SectionTracker( NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent )
        :   TrackerBase( nameAndLocation, ctx, parent )
        {
            if( parent ) {
                while( !parent->isSectionTracker() )
                    parent = &parent->parent();

                SectionTracker& parentSection = static_cast<SectionTracker&>( *parent );
                addNextFilters( parentSection.m_filters );
            }
        }
        virtual ~SectionTracker();

        virtual bool isSectionTracker() const override { return true; }

        static SectionTracker& acquire( TrackerContext& ctx, NameAndLocation const& nameAndLocation ) {
            std::shared_ptr<SectionTracker> section;

            ITracker& currentTracker = ctx.currentTracker();
            if( ITrackerPtr childTracker = currentTracker.findChild( nameAndLocation ) ) {
                assert( childTracker );
                assert( childTracker->isSectionTracker() );
                section = std::static_pointer_cast<SectionTracker>( childTracker );
            }
            else {
                section = std::make_shared<SectionTracker>( nameAndLocation, ctx, &currentTracker );
                currentTracker.addChild( section );
            }
            if( !ctx.completedCycle() )
                section->tryOpen();
            return *section;
        }

        void tryOpen() {
            if( !isComplete() && (m_filters.empty() || m_filters[0].empty() ||  m_filters[0] == m_nameAndLocation.name ) )
                open();
        }

        void addInitialFilters( std::vector<std::string> const& filters ) {
            if( !filters.empty() ) {
                m_filters.push_back(""); // Root - should never be consulted
                m_filters.push_back(""); // Test Case - not a section filter
                m_filters.insert( m_filters.end(), filters.begin(), filters.end() );
            }
        }
        void addNextFilters( std::vector<std::string> const& filters ) {
            if( filters.size() > 1 )
                m_filters.insert( m_filters.end(), ++filters.begin(), filters.end() );
        }
    };

    class IndexTracker : public TrackerBase {
        int m_size;
        int m_index = -1;
    public:
        IndexTracker( NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent, int size )
        :   TrackerBase( nameAndLocation, ctx, parent ),
            m_size( size )
        {}
        virtual ~IndexTracker();

        virtual bool isIndexTracker() const override { return true; }

        static IndexTracker& acquire( TrackerContext& ctx, NameAndLocation const& nameAndLocation, int size ) {
            std::shared_ptr<IndexTracker> tracker;

            ITracker& currentTracker = ctx.currentTracker();
            if( ITrackerPtr childTracker = currentTracker.findChild( nameAndLocation ) ) {
                assert( childTracker );
                assert( childTracker->isIndexTracker() );
                tracker = std::static_pointer_cast<IndexTracker>( childTracker );
            }
            else {
                tracker = std::make_shared<IndexTracker>( nameAndLocation, ctx, &currentTracker, size );
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

        virtual void close() override {
            TrackerBase::close();
            if( m_runState == CompletedSuccessfully && m_index < m_size-1 )
                m_runState = Executing;
        }
    };

    inline ITracker& TrackerContext::startRun() {
        m_rootTracker = std::make_shared<SectionTracker>( NameAndLocation( "{root}", CATCH_INTERNAL_LINEINFO ), *this, nullptr );
        m_currentTracker = nullptr;
        m_runState = Executing;
        return *m_rootTracker;
    }

} // namespace TestCaseTracking

using TestCaseTracking::ITracker;
using TestCaseTracking::TrackerContext;
using TestCaseTracking::SectionTracker;
using TestCaseTracking::IndexTracker;

} // namespace Catch

CATCH_INTERNAL_UNSUPPRESS_ETD_WARNINGS

#endif // TWOBLUECUBES_CATCH_TEST_CASE_TRACKER_HPP_INCLUDED
