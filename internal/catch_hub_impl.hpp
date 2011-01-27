/*
 *  catch_hub_impl.hpp
 *  Catch
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#include "catch_hub.h"
#include "catch_reporter_registry.hpp"
#include "catch_test_case_registry_impl.hpp"
#include "catch_runner_impl.hpp"
#include "catch_stream.hpp"

namespace Catch
{    
    ///////////////////////////////////////////////////////////////////////////
    Hub::Hub
    ()
    :   m_reporterRegistry( new ReporterRegistry ),
        m_testCaseRegistry( new TestRegistry )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    Hub& Hub::me
    ()
    {
        static Hub hub;
        return hub;
    }

    ///////////////////////////////////////////////////////////////////////////
    void Hub::setRunner( IRunner* runner )
    {
        me().m_runner = runner;
    }
    ///////////////////////////////////////////////////////////////////////////
    void Hub::setResultCapture( IResultCapture* resultCapture )
    {
        me().m_resultCapture = resultCapture;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    IResultCapture& Hub::getResultCapture
    ()
    {
        return *me().m_resultCapture;
    }

    ///////////////////////////////////////////////////////////////////////////
    IRunner& Hub::getRunner
    ()
    {
        return *me().m_runner;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    IReporterRegistry& Hub::getReporterRegistry
    ()
    {
        return *me().m_reporterRegistry.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    ITestCaseRegistry& Hub::getTestCaseRegistry
    ()
    {
        return *me().m_testCaseRegistry.get();
    }
    
    ///////////////////////////////////////////////////////////////////////////
    std::streambuf* Hub::createStreamBuf
    (
        const std::string& streamName
    )
    {
        if( streamName == "stdout" ) return std::cout.rdbuf();
        if( streamName == "stderr" ) return std::cerr.rdbuf();
        if( streamName == "debug" ) return new StreamBufImpl<OutputDebugWriter>;
        
        throw std::domain_error( "Unknown stream: " + streamName );
    }

    struct GeneratorInfo
    {
        GeneratorInfo
        ( 
            std::size_t size
        )
        :   m_size( size ),
            m_currentIndex( 0 )
        {
        }
        
        bool moveNext
        ()
        {
            if( ++m_currentIndex == m_size )
            {
                m_currentIndex = 0;
                return false;
            }
            return true;
        }
        
        std::size_t getCurrentIndex
        ()
        const
        {
            return m_currentIndex;
        }
        
        std::size_t m_size;
        std::size_t m_currentIndex;
    };

    class GeneratorsForTest
    {

    public:
        GeneratorsForTest
        ()
        :  m_currentGenerator( 0 )
        {
        }
        
        ~GeneratorsForTest
        ()
        {
            deleteAll( m_generatorsInOrder );
        }
        
        GeneratorInfo& getGeneratorInfo
        (
            const std::string& fileInfo,
            std::size_t size
        )
        {
            std::map<std::string, GeneratorInfo*>::const_iterator it = m_generatorsByName.find( fileInfo );
            if( it == m_generatorsByName.end() )
            {
                GeneratorInfo* info = new GeneratorInfo( size );
                m_generatorsByName.insert( std::make_pair( fileInfo, info ) );
                m_generatorsInOrder.push_back( info );
                return *info;
            }
            return *it->second;
        }
        
        bool moveNext
        ()
        {
            std::vector<GeneratorInfo*>::const_iterator it = m_generatorsInOrder.begin();
            std::vector<GeneratorInfo*>::const_iterator itEnd = m_generatorsInOrder.end();
            for(; it != itEnd; ++it )
            {
                if( (*it)->moveNext() )
                    return true;
            }
            return false;

            /*
            if( !m_generatorsInOrder[m_currentGenerator]->moveNext() )
            {
                if( ++m_currentGenerator == m_generatorsInOrder.size() )
                {
                    m_currentGenerator = 0;
                    return false;
                }
            }
            return true;
             */
        }
        
    private:
        std::map<std::string, GeneratorInfo*> m_generatorsByName;
        std::vector<GeneratorInfo*> m_generatorsInOrder;
        std::size_t m_currentGenerator;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    GeneratorsForTest* Hub::findGeneratorsForCurrentTest
    ()
    {
        std::string testName = getResultCapture().getCurrentTestName();
        
        std::map<std::string, GeneratorsForTest*>::const_iterator it = 
            m_generatorsByTestName.find( testName );
        return it != m_generatorsByTestName.end()
            ? it->second
            : NULL;
    }
    ///////////////////////////////////////////////////////////////////////////
    GeneratorsForTest& Hub::getGeneratorsForCurrentTest
    ()
    {
        GeneratorsForTest* generators = findGeneratorsForCurrentTest();
        if( !generators )
        {
            std::string testName = getResultCapture().getCurrentTestName();
            generators = new GeneratorsForTest();
            m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
        }
        return *generators;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    size_t Hub::getGeneratorIndex
    (
        const std::string& fileInfo, 
        size_t totalSize 
    )
    {
        return me().getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
    }

    ///////////////////////////////////////////////////////////////////////////
    bool Hub::advanceGeneratorsForCurrentTest
    ()
    {
        GeneratorsForTest* generators = me().findGeneratorsForCurrentTest();
        return generators && generators->moveNext();
    }
}
