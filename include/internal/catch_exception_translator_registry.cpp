/*
 *  Created by Phil on 20/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_assertionhandler.h"
#include "catch_exception_translator_registry.h"

#ifdef __OBJC__
#import "Foundation/Foundation.h"
#endif

namespace Catch {

    ExceptionTranslatorRegistry::~ExceptionTranslatorRegistry() {
    }

    void ExceptionTranslatorRegistry::registerTranslator( const IExceptionTranslator* translator ) {
        m_translators.push_back( std::unique_ptr<const IExceptionTranslator>( translator ) );
    }

    std::string ExceptionTranslatorRegistry::translateActiveException() const {
        try {
#ifdef __OBJC__
            // In Objective-C try objective-c exceptions first
            @try {
                return tryTranslators();
            }
            @catch (NSException *exception) {
                return Catch::Detail::stringify( [exception description] );
            }
#else
            return tryTranslators();
#endif
        }
        catch( TestFailureException& ) {
            std::rethrow_exception(std::current_exception());
        }
        catch( std::exception& ex ) {
            return ex.what();
        }
        catch( std::string& msg ) {
            return msg;
        }
        catch( const char* msg ) {
            return msg;
        }
        catch(...) {
            return "Unknown exception";
        }
    }

    std::string ExceptionTranslatorRegistry::tryTranslators() const {
        if( m_translators.empty() )
            std::rethrow_exception(std::current_exception());
        else
            return m_translators[0]->translate( m_translators.begin()+1, m_translators.end() );
    }
}
