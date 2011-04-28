/*
 *  catch_approx.hpp
 *  Catch
 *
 *  Created by Phil on 28/04/2011.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_APPROX_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_APPROX_HPP_INCLUDED

#include "catch_capture.hpp"

#include <cmath>

namespace Catch
{
    
    // !TBD Need to clean this all up
#define CATCH_absTol 1e-10
#define CATCH_relTol 1e-10
    
    inline double catch_max( double x, double y )
    {
        return x > y ? x : y;
    }
    namespace Detail
    {
        class Approx
        {
        public:
            ///////////////////////////////////////////////////////////////////////////
            // !TBD more generic
            explicit Approx
            (
             double d
             )
            : m_d( d )
            {
            }
            
            ///////////////////////////////////////////////////////////////////////////
            template<typename T>
            friend bool operator == 
            (
             const T& lhs, 
             const Approx& rhs
             )
            {
                // !TBD Use proper tolerance
                // From: http://realtimecollisiondetection.net/blog/?p=89
                // see also: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
                return fabs( lhs - rhs.m_d ) <= catch_max( CATCH_absTol, CATCH_relTol * catch_max( fabs(lhs), fabs(rhs.m_d) ) );
            }
            
            ///////////////////////////////////////////////////////////////////////////
            template<typename T>
            friend bool operator != 
            (
             const T& lhs, 
             const Approx& rhs
             )
            {
                return ! operator==( lhs, rhs );
            }
            
            double m_d;
        };
    }
  
    ///////////////////////////////////////////////////////////////////////////////
    template<>
    inline std::string toString<Detail::Approx>
    (
        const Detail::Approx& value
    )
    {
        std::ostringstream oss;
        oss << "Approx( " << value.m_d << ")";
        return oss.str();
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_APPROX_HPP_INCLUDED
