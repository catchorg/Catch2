/*
 *  Created by Martin on 31/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_session.h"
#include "catch_commandline.h"
#include "catch_console_colour.h"
#include "catch_enforce.h"
#include "catch_list.h"
#include "catch_context.h"
#include "catch_run_context.h"
#include "catch_stream.h"
#include "catch_test_spec.h"
#include "catch_version.h"
#include "catch_interfaces_reporter.h"
#include "catch_random_number_generator.h"
#include "catch_startup_exception_registry.h"
#include "catch_text.h"
#include "catch_stream.h"
#include "catch_windows_h_proxy.h"
#include "../reporters/catch_reporter_listening.h"

#include <cstdlib>
#include <iomanip>

namespace Catch {

    namespace {
        const int MaxExitCode = 255;

        IStreamingReporterPtr createReporter(std::string const& reporterName, IConfigPtr const& config) {
            auto reporter = Catch::getRegistryHub().getReporterRegistry().create(reporterName, config);
            CATCH_ENFORCE(reporter, "No reporter registered with name: '" << reporterName << "'");

            return reporter;
        }

        IStreamingReporterPtr makeReporter(std::shared_ptr<Config> const& config) {
            if (Catch::getRegistryHub().getReporterRegistry().getListeners().empty()) {
                return createReporter(config->getReporterName(), config);
            }

            // On older platforms, returning std::unique_ptr<ListeningReporter>
            // when the return type is std::unique_ptr<IStreamingReporter>
            // doesn't compile without a std::move call. However, this causes
            // a warning on newer platforms. Thus, we have to work around
            // it a bit and downcast the pointer manually.
            auto ret = std::unique_ptr<IStreamingReporter>(new ListeningReporter);
            auto& multi = static_cast<ListeningReporter&>(*ret);
            auto const& listeners = Catch::getRegistryHub().getReporterRegistry().getListeners();
            for (auto const& listener : listeners) {
                multi.addListener(listener->create(Catch::ReporterConfig(config)));
            }
            multi.addReporter(createReporter(config->getReporterName(), config));
            return ret;
        }


        Catch::Totals runTests(std::shared_ptr<Config> const& config) {
            auto reporter = makeReporter(config);

            RunContext context(config, std::move(reporter));

            Totals totals;

            context.testGroupStarting(config->name(), 1, 1);

            TestSpec testSpec = config->testSpec();

            auto const& allTestCases = getAllTestCasesSorted(*config);
            for (auto const& testCase : allTestCases) {
                if (!context.aborting() && matchTest(testCase, testSpec, *config))
                    totals += context.runTest(testCase);
                else
                    context.reporter().skipTest(testCase);
            }

            if (config->warnAboutNoTests() && totals.testCases.total() == 0) {
                ReusableStringStream testConfig;

                bool first = true;
                for (const auto& input : config->getTestsOrTags()) {
                    if (!first) { testConfig << ' '; }
                    first = false;
                    testConfig << input;
                }

                context.reporter().noMatchingTestCases(testConfig.str());
                totals.error = -1;
            }

            context.testGroupEnded(config->name(), totals, 1, 1);
            return totals;
        }

        void applyFilenamesAsTags(Catch::IConfig const& config) {
            auto& tests = const_cast<std::vector<TestCase>&>(getAllTestCasesSorted(config));
            for (auto& testCase : tests) {
                auto tags = testCase.tags;

                std::string filename = testCase.lineInfo.file;
                auto lastSlash = filename.find_last_of("\\/");
                if (lastSlash != std::string::npos) {
                    filename.erase(0, lastSlash);
                    filename[0] = '#';
                }

                auto lastDot = filename.find_last_of('.');
                if (lastDot != std::string::npos) {
                    filename.erase(lastDot);
                }

                tags.push_back(std::move(filename));
                setTags(testCase, tags);
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
            m_startupExceptions = true;
            Colour colourGuard( Colour::Red );
            Catch::cerr() << "Errors occurred during startup!" << '\n';
            // iterate over all exceptions and notify user
            for ( const auto& ex_ptr : exceptions ) {
                try {
                    std::rethrow_exception(ex_ptr);
                } catch ( std::exception const& ex ) {
                    Catch::cerr() << Column( ex.what() ).indent(2) << '\n';
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
                << "\nCatch v" << libraryVersion() << "\n"
                << m_cli << std::endl
                << "For more detailed usage please see the project docs\n" << std::endl;
    }
    void Session::libIdentify() {
        Catch::cout()
                << std::left << std::setw(16) << "description: " << "A Catch test executable\n"
                << std::left << std::setw(16) << "category: " << "testframework\n"
                << std::left << std::setw(16) << "framework: " << "Catch Test\n"
                << std::left << std::setw(16) << "version: " << libraryVersion() << std::endl;
    }

    int Session::applyCommandLine( int argc, char const * const * argv ) {
        if( m_startupExceptions )
            return 1;

        auto result = m_cli.parse( clara::Args( argc, argv ) );
        if( !result ) {
            config();
            getCurrentMutableContext().setConfig(m_config);
            Catch::cerr()
                << Colour( Colour::Red )
                << "\nError(s) in input:\n"
                << Column( result.errorMessage() ).indent( 2 )
                << "\n\n";
            Catch::cerr() << "Run with -? for usage\n" << std::endl;
            return MaxExitCode;
        }

        if( m_configData.showHelp )
            showHelp();
        if( m_configData.libIdentify )
            libIdentify();
        m_config.reset();
        return 0;
    }

#if defined(CATCH_CONFIG_WCHAR) && defined(WIN32) && defined(UNICODE)
    int Session::applyCommandLine( int argc, wchar_t const * const * argv ) {

        char **utf8Argv = new char *[ argc ];

        for ( int i = 0; i < argc; ++i ) {
            int bufSize = WideCharToMultiByte( CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL );

            utf8Argv[ i ] = new char[ bufSize ];

            WideCharToMultiByte( CP_UTF8, 0, argv[i], -1, utf8Argv[i], bufSize, NULL, NULL );
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
            Catch::cout() << "...waiting for enter/ return before starting" << std::endl;
            static_cast<void>(std::getchar());
        }
        int exitCode = runInternal();
        if( ( m_configData.waitForKeypress & WaitForKeypress::BeforeExit ) != 0 ) {
            Catch::cout() << "...waiting for enter/ return before exiting, with code: " << exitCode << std::endl;
            static_cast<void>(std::getchar());
        }
        return exitCode;
    }

    clara::Parser const& Session::cli() const {
        return m_cli;
    }
    void Session::cli( clara::Parser const& newParser ) {
        m_cli = newParser;
    }
    ConfigData& Session::configData() {
        return m_configData;
    }
    Config& Session::config() {
        if( !m_config )
            m_config = std::make_shared<Config>( m_configData );
        return *m_config;
    }

    int Session::runInternal() {
        if( m_startupExceptions )
            return 1;

        if (m_configData.showHelp || m_configData.libIdentify) {
            return 0;
        }

        CATCH_TRY {
            config(); // Force config to be constructed

            seedRng( *m_config );

            if( m_configData.filenamesAsTags )
                applyFilenamesAsTags( *m_config );

            // Handle list request
            if( Option<std::size_t> listed = list( m_config ) )
                return static_cast<int>( *listed );

            auto totals = runTests( m_config );
            // Note that on unices only the lower 8 bits are usually used, clamping
            // the return value to 255 prevents false negative when some multiple
            // of 256 tests has failed
            return (std::min) (MaxExitCode, (std::max) (totals.error, static_cast<int>(totals.assertions.failed)));
        }
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
        catch( std::exception& ex ) {
            Catch::cerr() << ex.what() << std::endl;
            return MaxExitCode;
        }
#endif
    }

} // end namespace Catch
