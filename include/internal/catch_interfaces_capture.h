/*
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED

#include <string>
#include "catch_result_type.h"
#include "catch_totals.hpp"

namespace Catch {

    class TestCaseInfo;
    class ScopedInfo;
    class ResultInfoBuilder;
    class ResultInfo;

    struct IResultCapture {
    
        virtual ~IResultCapture();
        
        virtual void testEnded( const ResultInfo& result ) = 0;
        virtual bool sectionStarted(    const std::string& name, 
                                        const std::string& description, 
                                        const SourceLineInfo& lineInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( const std::string& name, const Counts& assertions ) = 0;
        virtual void pushScopedInfo( ScopedInfo* scopedInfo ) = 0;
        virtual void popScopedInfo( ScopedInfo* scopedInfo ) = 0;
        virtual bool shouldDebugBreak() const = 0;
        
        virtual ResultAction::Value acceptResult( bool result ) = 0;
        virtual ResultAction::Value acceptResult( ResultWas::OfType result ) = 0;
        virtual ResultAction::Value acceptExpression( const ResultInfoBuilder& resultInfo ) = 0;
        virtual void acceptMessage( const std::string& msg ) = 0;
        
        virtual std::string getCurrentTestName() const = 0;        
        virtual const ResultInfo* getLastResult() const = 0;        
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
