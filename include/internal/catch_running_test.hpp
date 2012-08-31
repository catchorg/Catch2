/*  
 *  Created by Phil Nash on 4/5/2012
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_INTERNAL_CATCH_RUNNING_TEST_HPP_INCLUDED
#define TWOBLUECUBES_INTERNAL_CATCH_RUNNING_TEST_HPP_INCLUDED

#include "catch_test_case_info.h"
#include "catch_section_info.hpp"

namespace Catch {

    class RunningTest {
    
        enum RunStatus {
            NothingRun,
            EncounteredASection,
            RanAtLeastOneSection,
            RanToCompletionWithSections,
            RanToCompletionWithNoSections
        };
        
    public:
        explicit RunningTest( const TestCaseInfo* info = NULL )
        :   m_info( info ),
            m_runStatus( RanAtLeastOneSection ),
            m_currentSection( &m_rootSection ),
            m_changed( false )
        {}
        
        bool wasSectionSeen() const {
            return  m_runStatus == RanAtLeastOneSection || 
                    m_runStatus == RanToCompletionWithSections;
        }
        
        bool isBranchSection() const {
            return  m_currentSection &&
                    m_currentSection->isBranch();
        }

        bool hasSections() const {
            return  m_runStatus == RanAtLeastOneSection ||
                    m_runStatus == RanToCompletionWithSections ||
                    m_runStatus == EncounteredASection;
        }

        void reset() {
            m_runStatus = NothingRun;
            m_changed = false;
            m_lastSectionToRun = NULL;
        }
        
        void ranToCompletion() {
            if( m_runStatus == RanAtLeastOneSection ||
                m_runStatus == EncounteredASection ) {
                m_runStatus = RanToCompletionWithSections;
                if( m_lastSectionToRun ) {
                    m_lastSectionToRun->ranToCompletion();
                    m_changed = true;
                }
            }
            else {
                m_runStatus = RanToCompletionWithNoSections;
            }
        }
        
        bool addSection( const std::string& name ) {
            if( m_runStatus == NothingRun )
                m_runStatus = EncounteredASection;
            
            SectionInfo* thisSection = m_currentSection->findSubSection( name );
            if( !thisSection ) {
                thisSection = m_currentSection->addSubSection( name );
                m_changed = true;
            }
            
            if( !wasSectionSeen() && thisSection->shouldRun() ) {
                m_currentSection = thisSection;
                m_lastSectionToRun = NULL;
                return true;
            }
            return false;
        }
        
        void endSection( const std::string& ) {
            if( m_currentSection->ran() ) {
                m_runStatus = RanAtLeastOneSection;
                m_changed = true;
            }
            else if( m_runStatus == EncounteredASection ) {
                m_runStatus = RanAtLeastOneSection;
                m_lastSectionToRun = m_currentSection;
            }
            m_currentSection = m_currentSection->getParent();
        }
        
        const TestCaseInfo& getTestCaseInfo() const {
            return *m_info;
        }
        
        bool hasUntestedSections() const {
            return  m_runStatus == RanAtLeastOneSection ||
                    ( m_rootSection.hasUntestedSections() && m_changed );
        }
        
    private:
        const TestCaseInfo* m_info;
        RunStatus m_runStatus;
        SectionInfo m_rootSection;
        SectionInfo* m_currentSection;
        SectionInfo* m_lastSectionToRun;
        bool m_changed;
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_RUNNING_TEST_HPP_INCLUDED
