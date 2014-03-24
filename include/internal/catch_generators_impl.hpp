/*
 *  Created by Phil on 28/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED

#include "catch_interfaces_generators.h"

#include "catch_common.h"

#include <vector>
#include <string>
#include <map>

namespace Catch {

    struct GeneratorInfo : IGeneratorInfo {

        GeneratorInfo( std::size_t size )
        :   m_size( size ),
            m_currentIndex( 0 )
        {}

        bool moveNext() {
            if( ++m_currentIndex == m_size ) {
                m_currentIndex = 0;
                return false;
            }
            return true;
        }

        std::size_t getCurrentIndex() const {
            return m_currentIndex;
        }

        std::size_t m_size;
        std::size_t m_currentIndex;
    };

    ///////////////////////////////////////////////////////////////////////////

    class GeneratorsForTest : public IGeneratorsForTest {

    public:
        ~GeneratorsForTest() {
            deleteAll( m_generatorsInOrder );
        }

        IGeneratorInfo& getGeneratorInfo( std::string const& fileInfo, std::size_t size ) {
            std::map<std::string, IGeneratorInfo*>::const_iterator it = m_generatorsByName.find( fileInfo );
            if( it == m_generatorsByName.end() ) {
                IGeneratorInfo* info = new GeneratorInfo( size );
                m_generatorsByName.insert( std::make_pair( fileInfo, info ) );
                m_generatorsInOrder.push_back( info );
                return *info;
            }
            return *it->second;
        }

        bool moveNext() {
            std::vector<IGeneratorInfo*>::const_iterator it = m_generatorsInOrder.begin();
            std::vector<IGeneratorInfo*>::const_iterator itEnd = m_generatorsInOrder.end();
            for(; it != itEnd; ++it ) {
                if( (*it)->moveNext() )
                    return true;
            }
            return false;
        }

    private:
        std::map<std::string, IGeneratorInfo*> m_generatorsByName;
        std::vector<IGeneratorInfo*> m_generatorsInOrder;
    };

    INTERNAL_CATCH_INLINE IGeneratorsForTest* createGeneratorsForTest()
    {
        return new GeneratorsForTest();
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED
