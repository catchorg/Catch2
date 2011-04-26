/*
 *  catch_interfaces_capture.h
 *  Catch
 *
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED

#include <string>
#include "catch_result_type.h"

namespace Catch
{    
    class TestCaseInfo;
    class ScopedInfo;
    class MutableResultInfo;

    struct IResultCapture
    {
        virtual ~IResultCapture
        ()
        {}
        
        virtual void testEnded
            (   const ResultInfo& result 
            ) = 0;
        virtual bool sectionStarted
            (   const std::string& name, 
                const std::string& description, 
                const std::string& filename,
                std::size_t line,
                std::size_t& successes, 
                std::size_t& failures 
            ) = 0;
        virtual void sectionEnded
            (   const std::string& name, 
                std::size_t successes, 
                std::size_t failures 
            ) = 0;
        virtual void pushScopedInfo
            (   ScopedInfo* scopedInfo 
            ) = 0;
        virtual void popScopedInfo
            (   ScopedInfo* scopedInfo 
            ) = 0;
        virtual bool shouldDebugBreak
            () const = 0;
        
        virtual ResultAction::Value acceptResult
            (   bool result 
            ) = 0;
        virtual ResultAction::Value acceptResult
            (   ResultWas::OfType result
            ) = 0;
        virtual ResultAction::Value acceptExpression
            (   const MutableResultInfo& resultInfo 
            ) = 0;
        virtual void acceptMessage
            (   const std::string& msg 
            ) = 0;
        
        virtual std::string getCurrentTestName
            () const = 0;
        
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED
