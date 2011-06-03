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
#include <limits>

namespace Catch
{
    namespace Detail
    {
        class Approx
        {
        public:
            ///////////////////////////////////////////////////////////////////////////
            explicit Approx
            (
                double d
            )
            :   m_epsilon( std::numeric_limits<float>::epsilon()*100 ),
                m_scale( 1.0 ),
                m_d( d )
            {
            }
            
            ///////////////////////////////////////////////////////////////////////////
            friend bool operator == 
            (
                double lhs,
                const Approx& rhs
            )
            {
                // Thanks to Richard Harris for his help refining this formula
                return fabs( lhs - rhs.m_d ) < rhs.m_epsilon * (rhs.m_scale + (std::max)( fabs(lhs), fabs(rhs.m_d) ) );
            }
            
            ///////////////////////////////////////////////////////////////////////////
            friend bool operator == 
            (
                const Approx& lhs,
                double rhs
            )
            {
                return operator==( rhs, lhs );
            }
            
            ///////////////////////////////////////////////////////////////////////////
            friend bool operator != 
            (
                double lhs, 
                const Approx& rhs
            )
            {
                return !operator==( lhs, rhs );
            }

            ///////////////////////////////////////////////////////////////////////////
            friend bool operator != 
            (
                const Approx& lhs,
                double rhs
            )
            {
                return !operator==( rhs, lhs );
            }
            
            
            ///////////////////////////////////////////////////////////////////////////
            Approx& epsilon
            (
                double newEpsilon
            )
            {
                m_epsilon = newEpsilon;
                return *this;
            }
            
            ///////////////////////////////////////////////////////////////////////////
            Approx& scale
            (
                double newScale
            )
            {
                m_scale = newScale;
                return *this;
            }
            
            ///////////////////////////////////////////////////////////////////////////
            std::string toString() const
            {
                std::ostringstream oss;
                oss << "Approx( " << m_d << ")";
                return oss.str();
            }
            
        private:
            double m_epsilon;
            double m_scale;
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
        return value.toString();
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_APPROX_HPP_INCLUDED
