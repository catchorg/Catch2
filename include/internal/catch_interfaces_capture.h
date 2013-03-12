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
#include "catch_common.h"

namespace Catch {

    class TestCaseInfo;
    class ScopedInfo;
    class ExpressionResultBuilder;
    class AssertionResult;
    struct AssertionInfo;

    struct IResultCapture {
    
        virtual ~IResultCapture();
        
        virtual void testEnded( const AssertionResult& result ) = 0;
        virtual bool sectionStarted(    const std::string& name, 
                                        const std::string& description, 
                                        const SourceLineInfo& lineInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( const std::string& name, const Counts& assertions ) = 0;
        virtual void pushScopedInfo( ScopedInfo* scopedInfo ) = 0;
        virtual void popScopedInfo( ScopedInfo* scopedInfo ) = 0;
        virtual bool shouldDebugBreak() const = 0;
        
        virtual ResultAction::Value acceptExpression( const ExpressionResultBuilder& assertionResult, const AssertionInfo& assertionInfo ) = 0;
        
        virtual std::string getCurrentTestName() const = 0;        
        virtual const AssertionResult* getLastResult() const = 0;        
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
