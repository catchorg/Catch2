/*
 *  Created by Phil on 6th April 2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_H_INCLUDED
#define TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_H_INCLUDED

#include "catch_interfaces_reporter.h"

namespace Catch
{
    class LegacyReporterAdapter : public SharedImpl<IStreamingReporter>
    {
    public:
        LegacyReporterAdapter( Ptr<IReporter> const& legacyReporter );
        virtual ~LegacyReporterAdapter();

        virtual ReporterPreferences getPreferences() const;
        virtual void noMatchingTestCases( std::string const& );
        virtual void testRunStarting( TestRunInfo const& );
        virtual void testGroupStarting( GroupInfo const& groupInfo );
        virtual void testCaseStarting( TestCaseInfo const& testInfo );
        virtual void sectionStarting( SectionInfo const& sectionInfo );
        virtual void assertionStarting( AssertionInfo const& );
        virtual bool assertionEnded( AssertionStats const& assertionStats );
        virtual void sectionEnded( SectionStats const& sectionStats );
        virtual void testCaseEnded( TestCaseStats const& testCaseStats );
        virtual void testGroupEnded( TestGroupStats const& testGroupStats );
        virtual void testRunEnded( TestRunStats const& testRunStats );

    private:
        Ptr<IReporter> m_legacyReporter;
    };
}

#endif // TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_H_INCLUDED
