/*
 *  Created by Martin Moene on 2013-12-05.
 *  Copyright 2012 Martin Moene. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_COMPACT_H_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_COMPACT_H_INCLUDED


#include "catch_reporter_bases.hpp"


namespace Catch {

    struct CompactReporter : StreamingReporterBase<CompactReporter> {

        using StreamingReporterBase::StreamingReporterBase;

        ~CompactReporter() override;

        static std::string getDescription();

        ReporterPreferences getPreferences() const override;

        void noMatchingTestCases(std::string const& spec) override;

        void assertionStarting(AssertionInfo const&) override;

        bool assertionEnded(AssertionStats const& _assertionStats) override;

        void sectionEnded(SectionStats const& _sectionStats) override;

        void testRunEnded(TestRunStats const& _testRunStats) override;

    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_COMPACT_H_INCLUDED
