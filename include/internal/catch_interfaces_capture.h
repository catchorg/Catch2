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
#include <chrono>

#include "catch_stringref.h"
#include "catch_result_type.h"

namespace Catch {

    class AssertionResult;
    struct AssertionInfo;
    struct SectionInfo;
    struct SectionEndInfo;
    struct MessageInfo;
    struct MessageBuilder;
    struct Counts;
    struct AssertionReaction;
    struct SourceLineInfo;

    struct ITransientExpression;
    struct IGeneratorTracker;

#if defined(CATCH_CONFIG_ENABLE_BENCHMARKING)
    struct BenchmarkInfo;
    template <typename Duration = std::chrono::duration<double, std::nano>>
    struct BenchmarkStats;
#endif // CATCH_CONFIG_ENABLE_BENCHMARKING

    struct IResultCapture {

        virtual ~IResultCapture();

        virtual bool sectionStarted(    SectionInfo const& sectionInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( SectionEndInfo const& endInfo ) = 0;
        virtual void sectionEndedEarly( SectionEndInfo const& endInfo ) = 0;

        virtual auto acquireGeneratorTracker( SourceLineInfo const& lineInfo ) -> IGeneratorTracker& = 0;

#if defined(CATCH_CONFIG_ENABLE_BENCHMARKING)
        virtual void benchmarkPreparing( std::string const& name ) = 0;
        virtual void benchmarkStarting( BenchmarkInfo const& info ) = 0;
        virtual void benchmarkEnded( BenchmarkStats<> const& stats ) = 0;
        virtual void benchmarkFailed( std::string const& error ) = 0;
#endif // CATCH_CONFIG_ENABLE_BENCHMARKING

        virtual void pushScopedMessage( MessageInfo const& message ) = 0;
        virtual void popScopedMessage( MessageInfo const& message ) = 0;

        virtual void emplaceUnscopedMessage( MessageBuilder const& builder ) = 0;

        virtual void handleFatalErrorCondition( StringRef message ) = 0;

        virtual void handleExpr
                (   AssertionInfo const& info,
                    ITransientExpression const& expr,
                    AssertionReaction& reaction ) = 0;
        virtual void handleMessage
                (   AssertionInfo const& info,
                    ResultWas::OfType resultType,
                    StringRef const& message,
                    AssertionReaction& reaction ) = 0;
        virtual void handleUnexpectedExceptionNotThrown
                (   AssertionInfo const& info,
                    AssertionReaction& reaction ) = 0;
        virtual void handleUnexpectedInflightException
                (   AssertionInfo const& info,
                    std::string const& message,
                    AssertionReaction& reaction ) = 0;
        virtual void handleIncomplete
                (   AssertionInfo const& info ) = 0;
        virtual void handleNonExpr
                (   AssertionInfo const &info,
                    ResultWas::OfType resultType,
                    AssertionReaction &reaction ) = 0;



        virtual bool lastAssertionPassed() = 0;
        virtual void assertionPassed() = 0;

        // Deprecated, do not use:
        virtual std::string getCurrentTestName() const = 0;
        virtual const AssertionResult* getLastResult() const = 0;
        virtual void exceptionEarlyReported() = 0;
    };

    IResultCapture& getResultCapture();
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
