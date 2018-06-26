/*
 *  Created by Phil Nash on 15/6/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_generators.hpp"
#include "catch_random_number_generator.h"
#include "catch_singletons.hpp"
#include "catch_interfaces_capture.h"

#include <set>

namespace Catch {
namespace generators {

    GeneratorBase::~GeneratorBase() {}

    std::vector<size_t> randomiseIndices( size_t selectionSize, size_t sourceSize ) {

        assert( selectionSize <= sourceSize );
        std::vector<size_t> indices;
        indices.reserve( selectionSize );
        std::uniform_int_distribution<size_t> uid( 0, sourceSize-1 );

        std::set<size_t> seen;
        // !TBD: improve this algorithm
        while( indices.size() < selectionSize ) {
            auto index = uid( rng() );
            if( seen.insert( index ).second )
                indices.push_back( index );
        }
        return indices;
    }

    auto getTrackerContext() -> TrackerContext& {
        return getResultCapture().getTrackerContext();
    }

    IGeneratorTracker::~IGeneratorTracker() {}

    struct GeneratorTracker : TestCaseTracking::TrackerBase, IGeneratorTracker {
        size_t m_index = -1;
        GeneratorBasePtr m_generator;

        GeneratorTracker( TestCaseTracking::NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent )
        :   TrackerBase( nameAndLocation, ctx, parent )
        {}
        ~GeneratorTracker();

        static GeneratorTracker& acquire( TrackerContext& ctx, TestCaseTracking::NameAndLocation const& nameAndLocation ) {
            std::shared_ptr<GeneratorTracker> tracker;

            ITracker& currentTracker = ctx.currentTracker();
            if( TestCaseTracking::ITrackerPtr childTracker = currentTracker.findChild( nameAndLocation ) ) {
                assert( childTracker );
                assert( childTracker->isIndexTracker() );
                tracker = std::static_pointer_cast<GeneratorTracker>( childTracker );
            }
            else {
                tracker = std::make_shared<GeneratorTracker>( nameAndLocation, ctx, &currentTracker );
                currentTracker.addChild( tracker );
            }

            if( !ctx.completedCycle() && !tracker->isComplete() ) {
                if( tracker->m_runState != ExecutingChildren && tracker->m_runState != NeedsAnotherRun )
                    tracker->moveNext();
                tracker->open();
            }

            return *tracker;
        }

        void moveNext() {
            m_index++;
            m_children.clear();
        }

        // TrackerBase interface
        bool isIndexTracker() const override { return true; }
        auto hasGenerator() const -> bool override {
            return !!m_generator;
        }
        void close() override {
            TrackerBase::close();
            if( m_runState == CompletedSuccessfully && m_index < m_generator->size()-1 )
                m_runState = Executing;
        }

        // IGeneratorTracker interface
        auto getGenerator() const -> GeneratorBasePtr const& override {
            return m_generator;
        }
        void setGenerator( GeneratorBasePtr&& generator ) override {
            m_generator = std::move( generator );
        }
        auto getIndex() const -> size_t override {
            return m_index;
        }
    };
    GeneratorTracker::~GeneratorTracker() {}


    auto acquireGeneratorTracker( SourceLineInfo const& lineInfo ) -> IGeneratorTracker& {
        auto& trackerContext = getTrackerContext();
        GeneratorTracker& tracker = GeneratorTracker::acquire( trackerContext, TestCaseTracking::NameAndLocation( "generator", lineInfo ) );
        assert( tracker.isOpen() );
        return tracker;
    }


} // namespace generators
} // namespace Catch
