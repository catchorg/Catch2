
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEST_CASE_TRACKER_HPP_INCLUDED
#define CATCH_TEST_CASE_TRACKER_HPP_INCLUDED

#include <catch2/internal/catch_source_line_info.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>
#include <catch2/internal/catch_stringref.hpp>

#include <string>
#include <vector>

namespace Catch {
namespace TestCaseTracking {

    struct NameAndLocation {
        std::string name;
        SourceLineInfo location;

        NameAndLocation( std::string const& _name, SourceLineInfo const& _location );
        friend bool operator==(NameAndLocation const& lhs, NameAndLocation const& rhs) {
            return lhs.name == rhs.name
                && lhs.location == rhs.location;
        }
    };

    class ITracker;

    using ITrackerPtr = Catch::Detail::unique_ptr<ITracker>;

    class ITracker {
        NameAndLocation m_nameAndLocation;

        using Children = std::vector<ITrackerPtr>;

    protected:
        enum CycleState {
            NotStarted,
            Executing,
            ExecutingChildren,
            NeedsAnotherRun,
            CompletedSuccessfully,
            Failed
        };

        ITracker* m_parent = nullptr;
        Children m_children;
        CycleState m_runState = NotStarted;

    public:
        ITracker( NameAndLocation const& nameAndLoc, ITracker* parent ):
            m_nameAndLocation( nameAndLoc ),
            m_parent( parent )
        {}


        // static queries
        NameAndLocation const& nameAndLocation() const {
            return m_nameAndLocation;
        }
        ITracker* parent() const {
            return m_parent;
        }

        virtual ~ITracker(); // = default


        // dynamic queries

        //! Returns true if tracker run to completion (successfully or not)
        virtual bool isComplete() const = 0;
        //! Returns true if tracker run to completion succesfully
        bool isSuccessfullyCompleted() const;
        //! Returns true if tracker has started but hasn't been completed
        bool isOpen() const;
        //! Returns true iff tracker has started
        bool hasStarted() const;

        // actions
        virtual void close() = 0; // Successfully complete
        virtual void fail() = 0;
        void markAsNeedingAnotherRun();

        //! Register a nested ITracker
        void addChild( ITrackerPtr&& child );
        /**
         * Returns ptr to specific child if register with this tracker.
         *
         * Returns nullptr if not found.
         */
        ITracker* findChild( NameAndLocation const& nameAndLocation );
        //! Have any children been added?
        bool hasChildren() const {
            return !m_children.empty();
        }


        //! Marks tracker as executing a child, doing se recursively up the tree
        void openChild();

        /**
         * Returns true if the instance is a section tracker
         *
         * Subclasses should override to true if they are, replaces RTTI
         * for internal debug checks.
         */
        virtual bool isSectionTracker() const;
        /**
         * Returns true if the instance is a generator tracker
         *
         * Subclasses should override to true if they are, replaces RTTI
         * for internal debug checks.
         */
        virtual bool isGeneratorTracker() const;
    };

    class TrackerContext {

        enum RunState {
            NotStarted,
            Executing,
            CompletedCycle
        };

        ITrackerPtr m_rootTracker;
        ITracker* m_currentTracker = nullptr;
        RunState m_runState = NotStarted;

    public:

        ITracker& startRun();
        void endRun();

        void startCycle();
        void completeCycle();

        bool completedCycle() const;
        ITracker& currentTracker();
        void setCurrentTracker( ITracker* tracker );
    };

    class TrackerBase : public ITracker {
    protected:

        TrackerContext& m_ctx;

    public:
        TrackerBase( NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent );

        bool isComplete() const override;

        void open();

        void close() override;
        void fail() override;

    private:
        void moveToParent();
        void moveToThis();
    };

    class SectionTracker : public TrackerBase {
        std::vector<StringRef> m_filters;
        std::string m_trimmed_name;
    public:
        SectionTracker( NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent );

        bool isSectionTracker() const override;

        bool isComplete() const override;

        static SectionTracker& acquire( TrackerContext& ctx, NameAndLocation const& nameAndLocation );

        void tryOpen();

        void addInitialFilters( std::vector<std::string> const& filters );
        void addNextFilters( std::vector<StringRef> const& filters );
        //! Returns filters active in this tracker
        std::vector<StringRef> const& getFilters() const;
        //! Returns whitespace-trimmed name of the tracked section
        StringRef trimmedName() const;
    };

} // namespace TestCaseTracking

using TestCaseTracking::ITracker;
using TestCaseTracking::TrackerContext;
using TestCaseTracking::SectionTracker;

} // namespace Catch

#endif // CATCH_TEST_CASE_TRACKER_HPP_INCLUDED
