/*
 *  catch_exception_translator_registry.hpp
 *  Catch
 *
 *  Created by Phil on 20/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_EXCEPTION_TRANSLATOR_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXCEPTION_TRANSLATOR_HPP_INCLUDED

#include "catch_interfaces_exception.h"

namespace Catch
{    
    class ExceptionTranslatorRegistry : public IExceptionTranslatorRegistry
    {
        ///////////////////////////////////////////////////////////////////////
        ~ExceptionTranslatorRegistry
        ()
        {
            deleteAll( m_translators );
        }
        
        ///////////////////////////////////////////////////////////////////////
        virtual void registerTranslator
        (   
            IExceptionTranslator* translator 
        )
        {
            m_translators.push_back( translator );
        }
        
        ///////////////////////////////////////////////////////////////////////
        virtual std::string translateActiveException
        ()
        const
        {
            try
            {
                throw;
            }
            catch( std::exception& ex )
            {
                return ex.what();
            }
            catch( std::string& msg )
            {
                return msg;
            }
            catch( const char* msg )
            {
                return msg;
            }
            catch(...)
            {
                return tryTranslators( m_translators.begin() );
            }
        }
        
        ///////////////////////////////////////////////////////////////////////
        std::string tryTranslators
        (
            std::vector<IExceptionTranslator*>::const_iterator it
        ) 
        const
        {
            if( it == m_translators.end() )
                return "Unknown exception";
            
            try
            {
                return (*it)->translate();
            }
            catch(...)
            {
                return tryTranslators( it+1 );
            }
        }
        
    private:
        std::vector<IExceptionTranslator*> m_translators;
    };
}

#endif // TWOBLUECUBES_CATCH_EXCEPTION_TRANSLATOR_HPP_INCLUDED
