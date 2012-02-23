/*
 *  catch_self_test.hpp
 *  Catch
 *
 *  Created by Phil on 14/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED

#include "catch.hpp"

namespace Catch
{
    class EmbeddedRunner
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        EmbeddedRunner
        ()
        {
        }
        
        std::size_t runMatching
            (   const std::string& rawTestSpec );
        
        ///////////////////////////////////////////////////////////////////////////
        std::string getOutput
        ()
        {
            return m_output;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        const Totals& getTotals
        ()
        const
        {
            return m_totals;
        }
        
    private:
        Totals m_totals;
        std::string m_output;
    };

    class MetaTestRunner
    {
    public:
        struct Expected
        {
            enum Result
            {
                ToSucceed,
                ToFail
            };
        };
        
        ///////////////////////////////////////////////////////////////////////////
        static void runMatching
        (
            const std::string& testSpec, 
            Expected::Result expectedResult
        )
        {
            forEach(    Hub::getTestCaseRegistry().getMatchingTestCases( testSpec ), 
                        MetaTestRunner( expectedResult ) );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        MetaTestRunner
        (
            Expected::Result expectedResult
        )
        : m_expectedResult( expectedResult )
        {        
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void operator()
        (
            const TestCaseInfo& testCase
        )
        {
            EmbeddedRunner runner;
            runner.runMatching( testCase.getName() );
            Totals totals = runner.getTotals();
            switch( m_expectedResult )
            {
                case Expected::ToSucceed:
                    if( totals.assertions.failed > 0 )
                    {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to succeed but there was/ were " 
                             << totals.assertions.failed << " failure(s)" );
                    }
                    break;
                case Expected::ToFail:
                    if( totals.assertions.passed > 0 )
                    {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to fail but there was/ were " 
                             << totals.assertions.passed << " success(es)" );
                    }
                    break;
            }        
        };
        
    private:
        Expected::Result m_expectedResult;
    };
    

    struct LineInfoRegistry
    {
        static LineInfoRegistry& get
        ()
        {
            static LineInfoRegistry s_instance;
            return s_instance;
        }
        
        void registerLineInfo
        (
            const std::string& name, 
            const SourceLineInfo& info 
        )
        {
            m_registry.insert( std::make_pair( name, info ) );
        }
        
        const SourceLineInfo* find( const std::string& name ) const
        {
            std::map<std::string, SourceLineInfo>::const_iterator it = m_registry.find( name );
            return it == m_registry.end() ? NULL : &(it->second);
        }

        const std::string infoForName( const std::string& name ) const
        {
            std::map<std::string, SourceLineInfo>::const_iterator it = m_registry.find( name );
            if( it == m_registry.end() )
                return "";
            std::ostringstream oss;
            oss << it->second;
            return oss.str();
        }
        
        std::map<std::string, SourceLineInfo> m_registry;
    };
    
    struct LineInfoRegistrar
    {
        LineInfoRegistrar( const char* name, const SourceLineInfo& lineInfo )
        {
            LineInfoRegistry::get().registerLineInfo( name, lineInfo );
        }
    };
}

#define CATCH_REGISTER_LINE_INFO( name ) ::Catch::LineInfoRegistrar INTERNAL_CATCH_UNIQUE_NAME( lineRegistrar )( name, ::Catch::SourceLineInfo( __FILE__, __LINE__ ) );
#define CATCH_GET_LINE_INFO( name ) ::Catch::LineInfoRegistry::get().infoForName( name )

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
