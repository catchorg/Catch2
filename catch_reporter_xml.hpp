/*
 *  catch_reporter_xml.hpp
 *  Catch
 *
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED

#include "internal/catch_capture.hpp"
#include "internal/catch_reporter_registry.hpp"
#include <iostream>

namespace Catch
{
    class XmlReporter : public Catch::ITestReporter
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        XmlReporter( const ReporterConfig& config = ReporterConfig() )
        :   m_config( config )
        {
        }        

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription()
        {
            return "Reports test results as an XML document";
        }
        
    private: // ITestReporter

        virtual void StartTesting(){}
        virtual void EndTesting( std::size_t succeeded, std::size_t failed ){(succeeded, failed);}
        
        virtual void StartGroup( const std::string& groupName ){(groupName);}
        virtual void EndGroup( const std::string& groupName, std::size_t succeeded, std::size_t failed ){(groupName, succeeded, failed);}
        
        virtual void StartSection( const std::string& sectionName, const std::string description ){(sectionName,description);}
        virtual void EndSection( const std::string& sectionName, std::size_t succeeded, std::size_t failed ){(sectionName, succeeded, failed);}
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo )
        {
            m_config.stream() << "<TestCase name='" << testInfo.getName() << "'>\n";
            m_currentTestSuccess = true;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void Result( const Catch::ResultInfo& resultInfo )
        {
            if( !m_config.includeSuccessfulResults() && resultInfo.ok() )
                return;

            if( resultInfo.hasExpression() )
            {
                m_config.stream()   << "\t<Expression success='" << (resultInfo.ok() ? "true" : "false") << "' "
                << "filename='" << resultInfo.getFilename() << "' line='" << resultInfo.getLine() << "'>\n"
                << "\t\t<Original>" << resultInfo.getExpression() << "</Original>\n"
                << "\t\t<Expanded>" << resultInfo.getExpandedExpression() << "</Expanded>\n";
                m_currentTestSuccess |= resultInfo.ok();
            }
            switch( resultInfo.getResultType() )
            {
                case ResultWas::ThrewException:
                    if( resultInfo.hasExpression() )
                        m_config.stream() << "\t";
                    m_config.stream() << "\t<Exception>" << resultInfo.getMessage() << "</Exception>\n";
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Info:
                    m_config.stream() << "\t<Info>" << resultInfo.getMessage() << "</Info>\n";
                    break;
                case ResultWas::Warning:
                    m_config.stream() << "\t<Warning>" << resultInfo.getMessage() << "</Warning>\n";
                    break;
                case ResultWas::ExplicitFailure:
                    m_config.stream() << "\t<Failure>" << resultInfo.getMessage() << "</Failure>\n";
                    m_currentTestSuccess = false;
                    break;
            }            
            if( resultInfo.hasExpression() )
            {
                m_config.stream()   << "\t</Expression>\n";
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase( const Catch::TestCaseInfo&, const std::string& stdOut, const std::string& stdErr )
        {
            m_config.stream() << "\t<OverallResult success='" << (m_currentTestSuccess ? "true" : "false" ) << "/>\n";
            m_config.stream() << "</TestCase>" << std::endl;
        }    
                
    private:
        const ReporterConfig& m_config;
        bool m_currentTestSuccess;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED