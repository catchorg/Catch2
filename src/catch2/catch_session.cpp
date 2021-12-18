
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_session.hpp>
#include <catch2/internal/catch_console_colour.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_list.hpp>
#include <catch2/internal/catch_context.hpp>
#include <catch2/internal/catch_run_context.hpp>
#include <catch2/internal/catch_stream.hpp>
#include <catch2/catch_test_spec.hpp>
#include <catch2/catch_version.hpp>
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/internal/catch_startup_exception_registry.hpp>
#include <catch2/internal/catch_sharding.hpp>
#include <catch2/internal/catch_textflow.hpp>
#include <catch2/internal/catch_windows_h_proxy.hpp>
#include <catch2/reporters/catch_reporter_listening.hpp>
#include <catch2/interfaces/catch_interfaces_reporter_registry.hpp>
#include <catch2/interfaces/catch_interfaces_reporter_factory.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <set>

namespace Catch {

    namespace {
        const int MaxExitCode = 255;

        IStreamingReporterPtr createReporter(std::string const& reporterName, IConfig const* config) {
            auto reporter = Catch::getRegistryHub().getReporterRegistry().create(reporterName, config);
            CATCH_ENFORCE(reporter, "No reporter registered with name: '" << reporterName << '\'');

            return reporter;
        }

        IStreamingReporterPtr makeReporter(Config const* config) {
            if (Catch::getRegistryHub().getReporterRegistry().getListeners().empty()) {
                return createReporter(config->getReporterName(), config);
            }

            auto multi = Detail::make_unique<ListeningReporter>(config);
            auto const& listeners = Catch::getRegistryHub().getReporterRegistry().getListeners();
            for (auto const& listener : listeners) {
                multi->addListener(listener->create(Catch::ReporterConfig(config)));
            }
            multi->addReporter(createReporter(config->getReporterName(), config));
            return multi;
        }

        class TestGroup {
        public:
            explicit TestGroup(IStreamingReporterPtr&& reporter, Config const* config):
                m_reporter(reporter.get()),
                m_config{config},
                m_context{config, CATCH_MOVE(reporter)} {

                assert( m_config->testSpec().getInvalidSpecs().empty() &&
                        "Invalid test specs should be handled before running tests" );

                auto const& allTestCases = getAllTestCasesSorted(*m_config);
                auto const& testSpec = m_config->testSpec();
                if ( !testSpec.hasFilters() ) {
                    for ( auto const& test : allTestCases ) {
                        if ( !test.getTestCaseInfo().isHidden() ) {
                            m_tests.emplace( &test );
                        }
                    }
                } else {
                    m_matches =
                        testSpec.matchesByFilter( allTestCases, *m_config );
                    for ( auto const& match : m_matches ) {
                        m_tests.insert( match.tests.begin(),
                                        match.tests.end() );
                    }
                }

                m_tests = createShard(m_tests, m_config->shardCount(), m_config->shardIndex());
            }

            Totals execute() {
                Totals totals;
                for (auto const& testCase : m_tests) {
                    if (!m_context.aborting())
                        totals += m_context.runTest(*testCase);
                    else
                        m_reporter->skipTest(testCase->getTestCaseInfo());
                }

                for (auto const& match : m_matches) {
                    if (match.tests.empty()) {
                        m_unmatchedTestSpecs = true;
                        m_reporter->noMatchingTestCases( match.name );
                    }
                }

                return totals;
            }

            bool hadUnmatchedTestSpecs() const {
                return m_unmatchedTestSpecs;
            }


        private:
            IStreamingReporter* m_reporter;
            Config const* m_config;
            RunContext m_context;
            std::set<TestCaseHandle const*> m_tests;
            TestSpec::Matches m_matches;
            bool m_unmatchedTestSpecs = false;
        };

        void applyFilenamesAsTags() {
            for (auto const& testInfo : getRegistryHub().getTestCaseRegistry().getAllInfos()) {
                testInfo->addFilenameTag();
            }
        }

    } // anon namespace

    Session::Session() {
        static bool alreadyInstantiated = false;
        if( alreadyInstantiated ) {
            CATCH_TRY { CATCH_INTERNAL_ERROR( "Only one instance of Catch::Session can ever be used" ); }
            CATCH_CATCH_ALL { getMutableRegistryHub().registerStartupException(); }
        }

        // There cannot be exceptions at startup in no-exception mode.
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
        const auto& exceptions = getRegistryHub().getStartupExceptionRegistry().getExceptions();
        if ( !exceptions.empty() ) {
            config();
            getCurrentMutableContext().setConfig(m_config.get());

            m_startupExceptions = true;
            Colour colourGuard( Colour::Red );
            Catch::cerr() << "Errors occurred during startup!" << '\n';
            // iterate over all exceptions and notify user
            for ( const auto& ex_ptr : exceptions ) {
                try {
                    std::rethrow_exception(ex_ptr);
                } catch ( std::exception const& ex ) {
                    Catch::cerr() << TextFlow::Column( ex.what() ).indent(2) << '\n';
                }
            }
        }
#endif

        alreadyInstantiated = true;
        m_cli = makeCommandLineParser( m_configData );
    }
    Session::~Session() {
        Catch::cleanUp();
    }

    void Session::showHelp() const {
        Catch::cout()
                << "\nCatch v" << libraryVersion() << '\n'
                << m_cli << '\n'
                << "For more detailed usage please see the project docs\n\n" << std::flush;
    }
    void Session::libIdentify() {
        Catch::cout()
                << std::left << std::setw(16) << "description: " << "A Catch2 test executable\n"
                << std::left << std::setw(16) << "category: " << "testframework\n"
                << std::left << std::setw(16) << "framework: " << "Catch Test\n"
                << std::left << std::setw(16) << "version: " << libraryVersion() << '\n' << std::flush;
    }

    int Session::applyCommandLine( int argc, char const * const * argv ) {
        if( m_startupExceptions )
            return 1;

        auto result = m_cli.parse( Clara::Args( argc, argv ) );

        if( !result ) {
            config();
            getCurrentMutableContext().setConfig(m_config.get());
            Catch::cerr()
                << Colour( Colour::Red )
                << "\nError(s) in input:\n"
                << TextFlow::Column( result.errorMessage() ).indent( 2 )
                << "\n\n";
            Catch::cerr() << "Run with -? for usage\n\n" << std::flush;
            return MaxExitCode;
        }

        if( m_configData.showHelp )
            showHelp();
        if( m_configData.libIdentify )
            libIdentify();

        m_config.reset();
        return 0;
    }

#if defined(CATCH_CONFIG_WCHAR) && defined(_WIN32) && defined(UNICODE)
    int Session::applyCommandLine( int argc, wchar_t const * const * argv ) {

        char **utf8Argv = new char *[ argc ];

        for ( int i = 0; i < argc; ++i ) {
            int bufSize = WideCharToMultiByte( CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr );

            utf8Argv[ i ] = new char[ bufSize ];

            WideCharToMultiByte( CP_UTF8, 0, argv[i], -1, utf8Argv[i], bufSize, nullptr, nullptr );
        }

        int returnCode = applyCommandLine( argc, utf8Argv );

        for ( int i = 0; i < argc; ++i )
            delete [] utf8Argv[ i ];

        delete [] utf8Argv;

        return returnCode;
    }
#endif

    void Session::useConfigData( ConfigData const& configData ) {
        m_configData = configData;
        m_config.reset();
    }

    int Session::run() {
        if( ( m_configData.waitForKeypress & WaitForKeypress::BeforeStart ) != 0 ) {
            Catch::cout() << "...waiting for enter/ return before starting\n" << std::flush;
            static_cast<void>(std::getchar());
        }
        int exitCode = runInternal();
        if( ( m_configData.waitForKeypress & WaitForKeypress::BeforeExit ) != 0 ) {
            Catch::cout() << "...waiting for enter/ return before exiting, with code: " << exitCode << '\n' << std::flush;
            static_cast<void>(std::getchar());
        }
        return exitCode;
    }

    Clara::Parser const& Session::cli() const {
        return m_cli;
    }
    void Session::cli( Clara::Parser const& newParser ) {
        m_cli = newParser;
    }
    ConfigData& Session::configData() {
        return m_configData;
    }
    Config& Session::config() {
        if( !m_config )
            m_config = Detail::make_unique<Config>( m_configData );
        return *m_config;
    }

    int Session::runInternal() {
        if( m_startupExceptions )
            return 1;

        if (m_configData.showHelp || m_configData.libIdentify) {
            return 0;
        }

        if ( m_configData.shardIndex >= m_configData.shardCount ) {
            Catch::cerr() << "The shard count (" << m_configData.shardCount
                          << ") must be greater than the shard index ("
                          << m_configData.shardIndex << ")\n"
                          << std::flush;
            return 1;
        }

        CATCH_TRY {
            config(); // Force config to be constructed

            seedRng( *m_config );

            if (m_configData.filenamesAsTags) {
                applyFilenamesAsTags();
            }

            // Set up global config instance before we start calling into other functions
            getCurrentMutableContext().setConfig(m_config.get());

            // Create reporter(s) so we can route listings through them
            auto reporter = makeReporter(m_config.get());

            auto const& invalidSpecs = m_config->testSpec().getInvalidSpecs();
            if ( !invalidSpecs.empty() ) {
                for ( auto const& spec : invalidSpecs ) {
                    reporter->reportInvalidTestSpec( spec );
                }
                return 1;
            }


            // Handle list request
            if (list(*reporter, *m_config)) {
                return 0;
            }

            TestGroup tests { CATCH_MOVE(reporter), m_config.get() };
            auto const totals = tests.execute();

            if ( tests.hadUnmatchedTestSpecs()
                && m_config->warnAboutUnmatchedTestSpecs() ) {
                return 3;
            }

            if ( totals.testCases.total() == 0
                && !m_config->zeroTestsCountAsSuccess() ) {
                return 2;
            }

            // Note that on unices only the lower 8 bits are usually used, clamping
            // the return value to 255 prevents false negative when some multiple
            // of 256 tests has failed
            return (std::min) (MaxExitCode, (std::max) (totals.error, static_cast<int>(totals.assertions.failed)));
        }
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
        catch( std::exception& ex ) {
            Catch::cerr() << ex.what() << '\n' << std::flush;
            return MaxExitCode;
        }
#endif
    }

} // end namespace Catch
