
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/reporters/catch_reporter_sonarqube.hpp>

#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>

#include <map>

namespace Catch {

    namespace {
        std::string createRngSeedString(uint32_t seed) {
            ReusableStringStream sstr;
            sstr << "rng-seed=" << seed;
            return sstr.str();
        }
    }

    void SonarQubeReporter::testRunStarting(TestRunInfo const& testRunInfo) {
        CumulativeReporterBase::testRunStarting(testRunInfo);

        xml.writeComment( createRngSeedString( m_config->rngSeed() ) );
        xml.startElement("testExecutions");
        xml.writeAttribute("version"_sr, '1');
    }

    void SonarQubeReporter::writeRun( TestRunNode const& runNode ) {
        std::map<std::string, std::vector<TestCaseNode const*>> testsPerFile;

        for ( auto const& child : runNode.children ) {
            testsPerFile[child->value.testInfo->lineInfo.file].push_back(
                child.get() );
        }

        for ( auto const& kv : testsPerFile ) {
            writeTestFile( kv.first, kv.second );
        }
    }

    void SonarQubeReporter::writeTestFile(std::string const& filename, std::vector<TestCaseNode const*> const& testCaseNodes) {
        XmlWriter::ScopedElement e = xml.scopedElement("file");
        xml.writeAttribute("path"_sr, filename);

        for (auto const& child : testCaseNodes)
            writeTestCase(*child);
    }

    void SonarQubeReporter::writeTestCase(TestCaseNode const& testCaseNode) {
        // All test cases have exactly one section - which represents the
        // test case itself. That section may have 0-n nested sections
        assert(testCaseNode.children.size() == 1);
        SectionNode const& rootSection = *testCaseNode.children.front();
        writeSection("", rootSection, testCaseNode.value.testInfo->okToFail());
    }

    void SonarQubeReporter::writeSection(std::string const& rootName, SectionNode const& sectionNode, bool okToFail) {
        std::string name = trim(sectionNode.stats.sectionInfo.name);
        if (!rootName.empty())
            name = rootName + '/' + name;

        if ( sectionNode.hasAnyAssertions()
            || !sectionNode.stdOut.empty()
            ||  !sectionNode.stdErr.empty() ) {
            XmlWriter::ScopedElement e = xml.scopedElement("testCase");
            xml.writeAttribute("name"_sr, name);
            xml.writeAttribute("duration"_sr, static_cast<long>(sectionNode.stats.durationInSeconds * 1000));

            writeAssertions(sectionNode, okToFail);
        }

        for (auto const& childNode : sectionNode.childSections)
            writeSection(name, *childNode, okToFail);
    }

    void SonarQubeReporter::writeAssertions(SectionNode const& sectionNode, bool okToFail) {
        for (auto const& assertionOrBenchmark : sectionNode.assertionsAndBenchmarks) {
            if (assertionOrBenchmark.isAssertion()) {
                writeAssertion(assertionOrBenchmark.asAssertion(), okToFail);
            }
        }
    }

    void SonarQubeReporter::writeAssertion(AssertionStats const& stats, bool okToFail) {
        AssertionResult const& result = stats.assertionResult;
        if (!result.isOk()) {
            std::string elementName;
            if (okToFail) {
                elementName = "skipped";
            } else {
                switch (result.getResultType()) {
                case ResultWas::ThrewException:
                case ResultWas::FatalErrorCondition:
                    elementName = "error";
                    break;
                case ResultWas::ExplicitFailure:
                    elementName = "failure";
                    break;
                case ResultWas::ExpressionFailed:
                    elementName = "failure";
                    break;
                case ResultWas::DidntThrowException:
                    elementName = "failure";
                    break;

                    // We should never see these here:
                case ResultWas::Info:
                case ResultWas::Warning:
                case ResultWas::Ok:
                case ResultWas::Unknown:
                case ResultWas::FailureBit:
                case ResultWas::Exception:
                    elementName = "internalError";
                    break;
                }
            }

            XmlWriter::ScopedElement e = xml.scopedElement(elementName);

            ReusableStringStream messageRss;
            messageRss << result.getTestMacroName() << '(' << result.getExpression() << ')';
            xml.writeAttribute("message"_sr, messageRss.str());

            ReusableStringStream textRss;
            if (stats.totals.assertions.total() > 0) {
                textRss << "FAILED:\n";
                if (result.hasExpression()) {
                    textRss << '\t' << result.getExpressionInMacro() << '\n';
                }
                if (result.hasExpandedExpression()) {
                    textRss << "with expansion:\n\t" << result.getExpandedExpression() << '\n';
                }
            }

            if (!result.getMessage().empty())
                textRss << result.getMessage() << '\n';

            for (auto const& msg : stats.infoMessages)
                if (msg.type == ResultWas::Info)
                    textRss << msg.message << '\n';

            textRss << "at " << result.getSourceInfo();
            xml.writeText(textRss.str(), XmlFormatting::Newline);
        }
    }

} // end namespace Catch
