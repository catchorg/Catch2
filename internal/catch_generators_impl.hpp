/*
 *  catch_generators_impl.hpp
 *  Catch
 *
 *  Created by Phil on 28/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED

#include "catch_common.h"

#include <vector>
#include <string>
#include <map>

namespace Catch
{
    struct GeneratorInfo
    {
        ///////////////////////////////////////////////////////////////////////        
        GeneratorInfo
        ( 
            std::size_t size
        )
        :   m_size( size ),
            m_currentIndex( 0 )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////        
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
        
        ///////////////////////////////////////////////////////////////////////        
        std::size_t getCurrentIndex
        ()
        const
        {
            return m_currentIndex;
        }
        
        std::size_t m_size;
        std::size_t m_currentIndex;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    class GeneratorsForTest
    {
        
    public:
        ///////////////////////////////////////////////////////////////////////        
        ~GeneratorsForTest
        ()
        {
            deleteAll( m_generatorsInOrder );
        }
        
        ///////////////////////////////////////////////////////////////////////        
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
        
        ///////////////////////////////////////////////////////////////////////        
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
        }
        
    private:
        std::map<std::string, GeneratorInfo*> m_generatorsByName;
        std::vector<GeneratorInfo*> m_generatorsInOrder;
    };
    
} // end namespace Catch

#define INTERNAL_CATCH_LINESTR2( line ) #line
#define INTERNAL_CATCH_LINESTR( line ) INTERNAL_CATCH_LINESTR2( line )

#define INTERNAL_CATCH_GENERATE( expr ) expr.setFileInfo( __FILE__ "(" INTERNAL_CATCH_LINESTR( __LINE__ ) ")" )

#endif // TWOBLUECUBES_CATCH_GENERATORS_HPP_INCLUDED
