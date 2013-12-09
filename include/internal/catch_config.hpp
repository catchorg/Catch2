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
#include "catch_stream.hpp"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace CatchOverrides {

    class ConfigGuard
    {
    public:
        ConfigGuard()
            : origConfig(Catch::getCurrentContext().getConfig())
        {}
        ~ConfigGuard()
        {
            Catch::getCurrentMutableContext().setConfig(origConfig);
        }
        const Catch::Ptr<Catch::IConfig const>& value() const {return origConfig;}
    private:
        ConfigGuard(const ConfigGuard&);
        ConfigGuard& operator=(const ConfigGuard&);
        
        const Catch::Ptr<Catch::IConfig const> origConfig;
    };

    template <typename T>
    class Config
    {
        typedef std::map<int, bool> LineData;
        typedef std::map<std::string, LineData> FileLineData;
    public:
        bool includeSuccessfulResults(const std::string& file, int c) const
        {
            bool result(false);
            FileLineData::const_iterator it = showSuccessfulTestsData.find(file);
            if( it != showSuccessfulTestsData.end() )
            {
                for( LineData::const_iterator lineIt = it->second.begin(); lineIt != it->second.end(); ++lineIt )
                {
                    if( c <= lineIt->first )
                        break;
                    result = lineIt->second;
                }
            }
            return result;
        }
        void insertSuccessfulResults(const std::string& file, int c, bool v)
        {
            FileLineData::iterator it = showSuccessfulTestsData.find(file);
            if( it == showSuccessfulTestsData.end() )
            {
                LineData tmp;
                tmp.insert(std::make_pair(c,v));
                showSuccessfulTestsData.insert(std::make_pair(file, tmp));
            }
            else
            {
                it->second.insert(std::make_pair(c,v));
            }
        }
        bool warnAboutMissingAssertions(const std::string& file, int c) const
        {
            bool result(false);
            FileLineData::const_iterator it = missingAssertionData.find(file);
            if( it != missingAssertionData.end() )
            {
                for( LineData::const_iterator lineIt = it->second.begin(); lineIt != it->second.end(); ++lineIt )
                {
                    if( c <= lineIt->first )
                        break;
                    result = lineIt->second;
                }
            }
            return result;
        }
        void insertMissingAssertions(const std::string& file, int c, bool v)
        {
            FileLineData::iterator it = missingAssertionData.find(file);
            if( it == missingAssertionData.end() )
            {
                LineData tmp;
                tmp.insert(std::make_pair(c,v));
                missingAssertionData.insert(std::make_pair(file, tmp));
            }
            else
            {
                it->second.insert(std::make_pair(c,v));
            }
        }
        static Config<T>& instance()
        {
            if( !s_instance )
            {
                s_instance = new Config<T>();
            }
            return *s_instance;
        }
    private:
        FileLineData showSuccessfulTestsData;
        FileLineData missingAssertionData;
        
        static Config<T>* s_instance;
    };
    template <typename T>
    Config<T>* Config<T>::s_instance = NULL;

    template <typename T>
    struct ConfigReset
    {
        ConfigReset( const std::string& file, int c )
        {
            Config<T>::instance().insertSuccessfulResults(file, c, false);
            Config<T>::instance().insertMissingAssertions(file, c, false);
        }
    };

    template <typename T>
    struct ConfigShowSuccessfulTests
    {
        template <typename U>
        ConfigShowSuccessfulTests( const std::string& file, int c, U v )
        {
            Config<T>::instance().insertSuccessfulResults(file, c, v ? true : false);
        }
    };

    template <typename T>
    struct ConfigWarnMissingAssertions
    {
        template <typename U>
        ConfigWarnMissingAssertions( const std::string& file, int c, U v )
        {
            Config<T>::instance().insertMissingAssertions(file, c, v ? true : false);
        }
    };
}

namespace Catch {

    struct ConfigData {

        ConfigData()
        :   listTests( false ),
            listTags( false ),
            listReporters( false ),
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
