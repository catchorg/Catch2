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

#include "catch_stringref.h"

namespace Catch {

    class AssertionResult;
    struct AssertionInfo;
    struct SectionInfo;
    struct SectionEndInfo;
    struct MessageInfo;
    struct Counts;
    struct BenchmarkInfo;
    struct BenchmarkStats;

    struct IResultCapture {

        virtual ~IResultCapture();

        virtual void assertionStarting( AssertionInfo const& info ) = 0;
        virtual void assertionEnded( AssertionResult const& result ) = 0;
        virtual bool sectionStarted(    SectionInfo const& sectionInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( SectionEndInfo const& endInfo ) = 0;
        virtual void sectionEndedEarly( SectionEndInfo const& endInfo ) = 0;

        virtual void benchmarkStarting( BenchmarkInfo const& info ) = 0;
        virtual void benchmarkEnded( BenchmarkStats const& stats ) = 0;

        virtual void pushScopedMessage( MessageInfo const& message ) = 0;
        virtual void popScopedMessage( MessageInfo const& message ) = 0;

        virtual std::string getCurrentTestName() const = 0;
        virtual const AssertionResult* getLastResult() const = 0;

        virtual void exceptionEarlyReported() = 0;

        virtual void handleFatalErrorCondition( StringRef message ) = 0;

        virtual bool lastAssertionPassed() = 0;
        virtual void assertionPassed() = 0;
        virtual void assertionRun() = 0;
    };

    IResultCapture& getResultCapture();
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
