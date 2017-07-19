/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "../internal/catch_interfaces_reporter.h"

namespace Catch {

    class MultipleReporters : public IStreamingReporter {
        typedef std::vector<IStreamingReporterPtr> Reporters;
        Reporters m_reporters;

    public:
        void add( IStreamingReporterPtr&& reporter );

    public: // IStreamingReporter

        virtual ReporterPreferences getPreferences() const override;

        virtual void noMatchingTestCases( std::string const& spec ) override;


        virtual void testRunStarting( TestRunInfo const& testRunInfo ) override;

        virtual void testGroupStarting( GroupInfo const& groupInfo ) override;


        virtual void testCaseStarting( TestCaseInfo const& testInfo ) override;

        virtual void sectionStarting( SectionInfo const& sectionInfo ) override;


        virtual void assertionStarting( AssertionInfo const& assertionInfo ) override;


        // The return value indicates if the messages buffer should be cleared:
        virtual bool assertionEnded( AssertionStats const& assertionStats ) override;

        virtual void sectionEnded( SectionStats const& sectionStats ) override;

        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) override;

        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) override;

        virtual void testRunEnded( TestRunStats const& testRunStats ) override;


        virtual void skipTest( TestCaseInfo const& testInfo ) override;

        virtual bool isMulti() const override;

    };

} // end namespace Catch
