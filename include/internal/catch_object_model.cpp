// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../reporters/catch_reporter_bases.hpp"
#include "catch_reporter_registrars.hpp"
#include "catch_interfaces_object_model.h"

namespace Catch {

    class ObjectModelListener;

    auto getObjectModelListenerInstance() -> ObjectModelListener*& {
        static ObjectModelListener* s_instance = nullptr;
        return s_instance;
    }

    class ObjectModelListener : public TestEventListenerBase, public IObjectModel {
        Option<AssertionStats> m_lastAssertionStats;

    public:
        using TestEventListenerBase::TestEventListenerBase;

        ObjectModelListener( ReporterConfig const& config )
        : TestEventListenerBase( config ) {
            getObjectModelListenerInstance() = this;
        }
        ~ObjectModelListener() override;

        // IStreamingReporter

        void assertionStarting(AssertionInfo const &) override {
            m_lastAssertionStats.reset();
        }
        auto assertionEnded( AssertionStats const& assertionStats ) -> bool override {
            m_lastAssertionStats = assertionStats;
            return false;
        }

        // IObjectModel
        auto getCurrentTestCaseInfo() -> Option<TestCaseInfo> const& override {
            return currentTestCaseInfo;
        }

        auto getSectionInfos() -> std::vector<SectionInfo> const& override {
            return m_sectionStack;
        }

        auto getLastAssertionStats() -> Option<AssertionStats> const& override {
            return m_lastAssertionStats;
        }
    };

    IObjectModel::~IObjectModel() {}

    ObjectModelListener::~ObjectModelListener() {
        getObjectModelListenerInstance() = nullptr;
    }

    auto getObjectModel() -> IObjectModel* {
        return getObjectModelListenerInstance();
    }
}

CATCH_REGISTER_LISTENER( Catch::ObjectModelListener );