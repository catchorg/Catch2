/*
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED

#include "catch_test_spec.h"
#include "catch_context.h"
#include "catch_interfaces_config.h"
#include "catch_stream.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace CatchOverrides {

    class ConfigGuard {
    public:
        ConfigGuard()
            : origConfig(Catch::getCurrentContext().getConfig())
        {}
        ~ConfigGuard() {
            Catch::getCurrentMutableContext().setConfig(origConfig);
        }
        const Catch::Ptr<Catch::IConfig const>& value() const {return origConfig;}
    private:
        ConfigGuard(const ConfigGuard&);
        ConfigGuard& operator=(const ConfigGuard&);
        
        const Catch::Ptr<Catch::IConfig const> origConfig;
    };

    enum OverrideType { OverrideUpdate, OverrideReset}; // Note: ordered; update must be before reset

    template <typename T>
    class ConfigLineData {
        typedef std::map<std::pair<int,OverrideType>, T> LineData;
        typedef std::map<std::string, LineData> FileLineData;
    public:
        T getValueForFileLine(const std::string& file, int line) const {
            T result(false);
            typename FileLineData::const_iterator it = m_data.find(file);
            if( it != m_data.end() ) {
                typename LineData::const_iterator start = it->second.begin();
                typename LineData::const_iterator end = it->second.end();
                for( typename LineData::const_iterator lineIt = it->second.begin(); lineIt != it->second.end(); ++lineIt ) {
                    const std::pair<int,OverrideType>& current = lineIt->first;
                    if( current.second == OverrideReset ) {
                        if( line == current.first ) {
                            result = lineIt->second;
                            end = lineIt;
                            break;
                        }
                        else
                            start = lineIt;
                    }
                }
                for( typename LineData::const_iterator lineIt = start; lineIt != end; ++lineIt ) {
                    const std::pair<int,OverrideType>& current = lineIt->first;
                    if( current.second == OverrideUpdate ) {
                        if( line < current.first )
                            break;
                        result = lineIt->second;
                    }
                }
            }
            return result;
        }
        void recordValueForFileLine(const std::string& file, OverrideType overRide, int line, const T& v)
        {
            typename FileLineData::iterator it = m_data.find(file);
            if( it == m_data.end() ) {
                LineData tmp;
                std::pair<int,OverrideType> current = std::make_pair(line, overRide);
                tmp.insert(std::make_pair(current,v));
                m_data.insert(std::make_pair(file, tmp));
            }
            else {
                std::pair<int,OverrideType> current = std::make_pair(line, overRide);
                typename LineData::iterator lineIt = it->second.find(current);
                if( lineIt == it->second.end() ) {
                    it->second.insert(std::make_pair(current,v));
                }
                else {
                    lineIt->second = v;
                }
            }
        }
    private:
        FileLineData m_data;
    };

    template <typename T>
    class Config {
        typedef std::map<std::pair<int,OverrideType>, bool> BoolLineData;
        typedef std::map<std::string, BoolLineData> FileBoolLineData;
        typedef std::map<std::pair<int,OverrideType>, int> LineData;
        typedef std::map<std::string, LineData> FileLineData;
        typedef std::multimap<std::pair<int,OverrideType>, std::string> StringLineData;
        typedef std::map<std::string, StringLineData> FileStringLineData;
    public:
        bool includeSuccessfulResults(const std::string& file, int line) const {
            return showSuccessfulTestsData.getValueForFileLine(file,line);
        }
        void insertSuccessfulResults(const std::string& file, OverrideType overRide, int line, bool v) {
            showSuccessfulTestsData.recordValueForFileLine(file, overRide, line, v);
        }
        bool warnAboutMissingAssertions(const std::string& file, int line) const {
            return missingAssertionData.getValueForFileLine(file,line);
        }
        void insertMissingAssertions(const std::string& file, OverrideType overRide, int line, bool v) {
            missingAssertionData.recordValueForFileLine(file, overRide, line, v);
        }
        int abortAfter(const std::string& file, int line) const {
            return abortAfterData.getValueForFileLine(file,line);
        }
        void insertAbortAfter(const std::string& file, OverrideType overRide, int line, int v) {
            abortAfterData.recordValueForFileLine(file, overRide, line, v);
        }
        std::vector<std::string> listOfTests(const std::string& file, int c) const {
            std::vector<std::string> result;
            FileStringLineData::const_iterator it = testData.find(file);
            if( it != testData.end() ) {
                StringLineData::const_iterator start = it->second.begin();
                StringLineData::const_iterator end = it->second.end();
                for( StringLineData::const_iterator lineIt = it->second.begin(); lineIt != it->second.end(); ++lineIt ) {
                    const std::pair<int,OverrideType>& current = lineIt->first;
                    if( current.second == OverrideReset ) {
                        if( c == current.first ) {
                            end = lineIt;
                            break;
                        }
                        else
                            start = lineIt;
                    }
                }
                for( StringLineData::const_iterator lineIt = start; lineIt != end; ++lineIt ) {
                    const std::pair<int,OverrideType>& current = lineIt->first;
                    if( current.second == OverrideUpdate ) {
                        if( c < current.first )
                            break;
                        result.push_back(lineIt->second);
                    }
                }
            }
            return result;
        }
        void insertTest(const std::string& file, OverrideType overRide, int c, const std::string& v) {
            FileStringLineData::iterator it = testData.find(file);
            if( it == testData.end() ) {
                StringLineData tmp;
                std::pair<int,OverrideType> current = std::make_pair(c, overRide);
                tmp.insert(std::make_pair(current,v));
                testData.insert(std::make_pair(file, tmp));
            }
            else {
                std::pair<int,OverrideType> current = std::make_pair(c, overRide);
                it->second.insert(std::make_pair(current,v));
            }
        }
        static Config<T>& instance() {
            if( !s_instance ) {
                s_instance = new Config<T>();
            }
            return *s_instance;
        }
    private:
        ConfigLineData<bool> showSuccessfulTestsData;
        ConfigLineData<bool> missingAssertionData;
        ConfigLineData<int> abortAfterData;
        FileStringLineData testData;
        
        static Config<T>* s_instance;
    };
    template <typename T>
    Config<T>* Config<T>::s_instance = NULL;

    template <typename T>
    struct ConfigReset {
        ConfigReset( const std::string& file, int c, int defaultAbortAfter ) {
            Config<T>::instance().insertSuccessfulResults(file, OverrideReset, c, false);
            Config<T>::instance().insertMissingAssertions(file, OverrideReset, c, false);
            Config<T>::instance().insertAbortAfter(file, OverrideReset, c, defaultAbortAfter);
            Config<T>::instance().insertTest(file, OverrideReset, c, "");
        }
    };

    template <typename T>
    struct ConfigShowSuccessfulTests {
        template <typename U>
        ConfigShowSuccessfulTests( const std::string& file, int c, U v ) {
            Config<T>::instance().insertSuccessfulResults(file, OverrideUpdate, c, v ? true : false);
        }
    };

    template <typename T>
    struct ConfigWarnMissingAssertions {
        template <typename U>
        ConfigWarnMissingAssertions( const std::string& file, int c, U v ) {
            Config<T>::instance().insertMissingAssertions(file, OverrideUpdate, c, v ? true : false);
        }
    };

    template <typename T>
    struct ConfigAbortAfter {
        template <typename U>
        ConfigAbortAfter( const std::string& file, int c, U v ) {
            Config<T>::instance().insertAbortAfter(file, OverrideUpdate, c, v);
        }
    };

    template <typename T>
    struct ConfigAddTest {
        template <typename U>
        ConfigAddTest( const std::string& file, int c, U v ) {
            Config<T>::instance().insertTest(file, OverrideUpdate, c, v);
        }
    };
}

namespace Catch {

    struct ConfigData {

        ConfigData()
        :   listTests( false ),
            listTags( false ),
            listReporters( false ),
            listTestNamesOnly( false ),
            showSuccessfulTests( false ),
            shouldDebugBreak( false ),
            noThrow( false ),
            showHelp( false ),
            abortAfter( -1 ),
            verbosity( Verbosity::Normal ),
            warnings( WarnAbout::Nothing ),
            showDurations( ShowDurations::DefaultForReporter )
        {}

        explicit ConfigData(const IConfig* other)
        :   listTests( false ),
            listTags( false ),
            listReporters( false ),
            showSuccessfulTests( other ? other->includeSuccessfulResults() : false ),
            shouldDebugBreak( false ),
            noThrow( other ? !other->allowThrows() : false ),
            showHelp( false ),
            abortAfter( -1 ),
            verbosity( Verbosity::Normal ),
            warnings( other ? (other->warnAboutMissingAssertions() ? WarnAbout::NoAssertions : WarnAbout::Nothing) : WarnAbout::Nothing ),
            showDurations( other ? other->showDurations() : ShowDurations::DefaultForReporter ),
            name( other ? other->name() : std::string() )
        {}

        bool listTests;
        bool listTags;
        bool listReporters;
        bool listTestNamesOnly;

        bool showSuccessfulTests;
        bool shouldDebugBreak;
        bool noThrow;
        bool showHelp;

        int abortAfter;

        Verbosity::Level verbosity;
        WarnAbout::What warnings;
        ShowDurations::OrNot showDurations;

        std::string reporterName;
        std::string outputFilename;
        std::string name;
        std::string processName;

        std::vector<std::string> testsOrTags;
    };


    class Config : public SharedImpl<IConfig> {
    private:
        Config( Config const& other );
        Config& operator = ( Config const& other );
        virtual void dummy();
    public:

        Config()
        :   m_os( std::cout.rdbuf() )
        {}

        Config( ConfigData const& data )
        :   m_data( data ),
            m_os( std::cout.rdbuf() )
        {
            if( !data.testsOrTags.empty() ) {
                std::string groupName;
                for( std::size_t i = 0; i < data.testsOrTags.size(); ++i ) {
                    if( i != 0 )
                        groupName += " ";
                    groupName += data.testsOrTags[i];
                }
                TestCaseFilters filters( groupName );
                for( std::size_t i = 0; i < data.testsOrTags.size(); ++i ) {
                    std::string filter = data.testsOrTags[i];
                    if( startsWith( filter, "[" ) || startsWith( filter, "~[" ) )
                        filters.addTags( filter );
                    else
                        filters.addFilter( TestCaseFilter( filter ) );
                }
                m_filterSets.push_back( filters );
            }
        }

        virtual ~Config() {
            m_os.rdbuf( std::cout.rdbuf() );
            m_stream.release();
        }

        void setFilename( std::string const& filename ) {
            m_data.outputFilename = filename;
        }

        std::string const& getFilename() const {
            return m_data.outputFilename ;
        }

        bool listTests() const { return m_data.listTests; }
        bool listTestNamesOnly() const { return m_data.listTestNamesOnly; }
        bool listTags() const { return m_data.listTags; }
        bool listReporters() const { return m_data.listReporters; }

        std::string getProcessName() const {
            return m_data.processName;
        }

        bool shouldDebugBreak() const {
            return m_data.shouldDebugBreak;
        }

        void setStreamBuf( std::streambuf* buf ) {
            m_os.rdbuf( buf ? buf : std::cout.rdbuf() );
        }

        void useStream( std::string const& streamName ) {
            Stream stream = createStream( streamName );
            setStreamBuf( stream.streamBuf );
            m_stream.release();
            m_stream = stream;
        }

        std::string getReporterName() const { return m_data.reporterName; }

        void addTestSpec( std::string const& testSpec ) {
            TestCaseFilters filters( testSpec );
            filters.addFilter( TestCaseFilter( testSpec ) );
            m_filterSets.push_back( filters );
        }

        int abortAfter() const {
            return m_data.abortAfter;
        }

        std::vector<TestCaseFilters> const& filters() const {
            return m_filterSets;
        }

        bool showHelp() const { return m_data.showHelp; }

        // IConfig interface
        virtual bool allowThrows() const        { return !m_data.noThrow; }
        virtual std::ostream& stream() const    { return m_os; }
        virtual std::string name() const        { return m_data.name.empty() ? m_data.processName : m_data.name; }
        virtual bool includeSuccessfulResults() const   { return m_data.showSuccessfulTests; }
        virtual bool warnAboutMissingAssertions() const { return m_data.warnings & WarnAbout::NoAssertions; }
        virtual ShowDurations::OrNot showDurations() const { return m_data.showDurations; }


    private:
        ConfigData m_data;

        Stream m_stream;
        mutable std::ostream m_os;
        std::vector<TestCaseFilters> m_filterSets;
    };


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
