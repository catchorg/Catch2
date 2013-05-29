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

namespace Catch {

    struct Include { enum WhichResults {
        FailedOnly, 
        SuccessfulResults
    }; };

    struct List{ enum What {
        None = 0,
        
        Reports = 1,
        Tests = 2,
        Tags = 4,
        All = Reports | Tests | Tags,

        WhatMask = 0xf,
        
        AsText = 0x10,
        AsXml = 0x20,
        
        AsMask = 0xf0
    }; };
    
    struct ConfigData {

        struct WarnAbout { enum What {
            Nothing = 0x00,
            NoAssertions = 0x01
        }; };

        ConfigData()
        :   listSpec( List::None ),
            shouldDebugBreak( false ),
            includeWhichResults( Include::FailedOnly ),
            cutoff( -1 ),
            allowThrows( true ),
            warnings( WarnAbout::Nothing )
        {}
        
        std::string reporter;
        std::string outputFilename;
        List::What listSpec;
        std::vector<TestCaseFilters> filters;
        bool shouldDebugBreak;
        std::string stream;
        Include::WhichResults includeWhichResults;
        std::string name;
        int cutoff;
        bool allowThrows;
        WarnAbout::What warnings;
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
            m_os( std::cout.rdbuf() ),
            m_filters( data.filters )
        {}
        
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
        
        bool listTests() const { return m_data.listSpec & List::Tests; }
        bool listTags() const { return m_data.listSpec & List::Tags; }
        bool listReporters() const { return m_data.listSpec & List::Reports; }
        
        std::string getName() const {
            return m_data.name;
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
        
        std::string getStreamName() const { return m_data.stream; }

        std::string getReporterName() const { return m_data.reporter; }

        void addTestSpec( std::string const& testSpec ) {
            TestCaseFilters filters( testSpec );
            filters.addFilter( TestCaseFilter( testSpec ) );
            m_data.filters.push_back( filters );
        }
                
        int abortAfter() const {
            return m_data.cutoff;
        }
        
        std::vector<TestCaseFilters> const& filters() const {
            return m_filters;
        }

        // IConfig interface
        virtual bool allowThrows() const        { return m_data.allowThrows; }
        virtual std::ostream& stream() const    { return m_os; }
        virtual std::string name() const        { return m_data.name; }
        virtual bool includeSuccessfulResults() const   { return m_data.includeWhichResults == Include::SuccessfulResults; }
        virtual bool warnAboutMissingAssertions() const { return m_data.warnings & ConfigData::WarnAbout::NoAssertions; }

    private:
        ConfigData m_data;
        
        Stream m_stream;
        mutable std::ostream m_os;
        std::vector<TestCaseFilters> m_filters;
    };
        
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
