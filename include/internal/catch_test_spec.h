/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED
#define TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED

#include <string>

namespace Catch {

    class TestSpec {
    public:
        TestSpec( const std::string& rawSpec )
        :   m_rawSpec( rawSpec ),
            m_isWildcarded( false ) {

            if( m_rawSpec[m_rawSpec.size()-1] == '*' ) {
                m_rawSpec = m_rawSpec.substr( 0, m_rawSpec.size()-1 );
                m_isWildcarded = true;
            }
        }

        bool matches ( const std::string& testName ) const {
            if( !m_isWildcarded )
                return m_rawSpec == testName;
            else
                return testName.size() >= m_rawSpec.size() && testName.substr( 0, m_rawSpec.size() ) == m_rawSpec;
        }

    private:
        std::string m_rawSpec;
        bool m_isWildcarded;
    };
}

#endif // TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED
