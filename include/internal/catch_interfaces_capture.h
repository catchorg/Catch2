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
#include "catch_common.h"

namespace Catch {

    class TestCase;
    class ExpressionResultBuilder;
    class AssertionResult;
    struct AssertionInfo;
    struct SectionInfo;
    struct MessageInfo;
    class ScopedMessageBuilder;
    struct Counts;

    struct IResultCapture {

        virtual ~IResultCapture();

        virtual void assertionEnded( AssertionResult const& result ) = 0;
        virtual bool sectionStarted(    SectionInfo const& sectionInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( SectionInfo const& name, Counts const& assertions, double _durationInSeconds ) = 0;
        virtual void pushScopedMessage( MessageInfo const& message ) = 0;
        virtual void popScopedMessage( MessageInfo const& message ) = 0;

        virtual bool shouldDebugBreak() const = 0;

        virtual ResultAction::Value acceptExpression( ExpressionResultBuilder const& assertionResult, AssertionInfo const& assertionInfo ) = 0;

        virtual std::string getCurrentTestName() const = 0;
        virtual const AssertionResult* getLastResult() const = 0;
    };

    struct NullResultCapture : public IResultCapture {

        virtual void assertionEnded( AssertionResult const& ) {}
        virtual bool sectionStarted(    SectionInfo const& ,
            Counts&  ) {return false;}
        virtual void sectionEnded( SectionInfo const& , Counts const& , double ) {}
        virtual void pushScopedMessage( MessageInfo const& ) {}
        virtual void popScopedMessage( MessageInfo const& ) {}

        virtual bool shouldDebugBreak() const {return false;}

        virtual ResultAction::Value acceptExpression( ExpressionResultBuilder const& , AssertionInfo const&  ) {return ResultAction::Abort;}

        virtual std::string getCurrentTestName() const {return std::string();}
        virtual const AssertionResult* getLastResult() const {return NULL;}
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
