/*
 *  catch_exception_interfaces.h
 *  Catch
 *
 *  Created by Phil on 20/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_EXCEPTIONS_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_EXCEPTIONS_H_INCLUDED

#include <string>
                                              
namespace Catch
{    
    typedef std::string(*exceptionTranslateFunction)();

    struct IExceptionTranslator
    {
        virtual ~IExceptionTranslator(){}
        virtual std::string translate() const = 0;
    };
    
    struct IExceptionTranslatorRegistry
    {
        virtual ~IExceptionTranslatorRegistry
        ()
        {}
        
        virtual void registerTranslator
            (   IExceptionTranslator* translator 
            ) = 0;
        virtual std::string translateActiveException
            () const = 0;
        
    };

    template<typename T>
    class ExceptionTranslator : public IExceptionTranslator
    {
    public:
        ExceptionTranslator()
        {
            Catch::Hub::getExceptionTranslatorRegistry().registerTranslator( this );
        }
        
        virtual std::string translate() const
        {
            try
            {
                throw;
            }
            catch( T& ex )
            {
                return translate( ex );
            }
        }
        
    protected:
        std::string translate( T& ex ) const;
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_EXCEPTIONS_H_INCLUDED
