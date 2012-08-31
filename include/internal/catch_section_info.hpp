/*  
 *  Created by Phil Nash on 4/5/2012
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_INTERNAL_CATCH_SECTION_INFO_HPP_INCLUDED
#define TWOBLUECUBES_INTERNAL_CATCH_SECTION_INFO_HPP_INCLUDED

#include "catch_common.h"

#include <map>
#include <string>

namespace Catch {

    class SectionInfo {
    public:
    
        enum Status {
            Root,
            Unknown,
            Branch,
            TestedBranch,
            TestedLeaf
        };
        
        SectionInfo( SectionInfo* parent )
        :   m_status( Unknown ),
            m_parent( parent )
        {}
        
        SectionInfo()
        :   m_status( Root ),
            m_parent( NULL )
        {}
        
        ~SectionInfo() {
            deleteAllValues( m_subSections );
        }
        
        bool shouldRun() const {
            return m_status < TestedBranch;
        }
        
        bool ran() {
            if( m_status < Branch ) {
                m_status = TestedLeaf;
                return true;
            }            
            return false;
        }

        bool isBranch() const {
            return m_status == Branch;
        }

        void ranToCompletion() {
            if( m_status == Branch && !hasUntestedSections() )
                m_status = TestedBranch;
        }
                
        SectionInfo* findSubSection( const std::string& name ) {
            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.find( name );
            return it != m_subSections.end()
                        ? it->second
                        : NULL;
        }
        
        SectionInfo* addSubSection( const std::string& name ) {
            SectionInfo* subSection = new SectionInfo( this );
            m_subSections.insert( std::make_pair( name, subSection ) );
            m_status = Branch;
            return subSection;
        }
        
        SectionInfo* getParent() {
            return m_parent;
        }
        
        bool hasUntestedSections() const {
            if( m_status == Unknown )
                return true;
            
            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.begin();
            std::map<std::string, SectionInfo*>::const_iterator itEnd = m_subSections.end();
            for(; it != itEnd; ++it ) {
                if( it->second->hasUntestedSections() )
                    return true;
            }
            return false;
        }
        
    private:
        Status m_status;
        std::map<std::string, SectionInfo*> m_subSections;
        SectionInfo* m_parent;
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_SECTION_INFO_HPP_INCLUDED
