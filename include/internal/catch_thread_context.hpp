//
/*
 *  Created by Phil on 24/11/2015.
 *  Copyright 2015 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_THREAD_CONTEXT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_THREAD_CONTEXT_HPP_INCLUDED

#include "catch_interfaces_capture.h"
#include "catch_notimplemented_exception.h"

namespace Catch {

    class ThreadContext : public IRunContext {

        IRunContext& m_parentContext;
        std::vector<MessageInfo> m_messages;
        std::vector<AssertionResult> m_assertions;

        ThreadContext( ThreadContext const& );
        void operator =( ThreadContext const& );

    public:
        ThreadContext( IRunContext& parentContext )
        :   m_parentContext( parentContext )
        {}
        virtual ~ThreadContext();

    private: // IRunContext

        virtual void assertionEnded( AssertionResult const& result ) CATCH_OVERRIDE {
            m_assertions.push_back( result );
        }

        virtual bool sectionStarted( SectionInfo const&, Counts& ) CATCH_OVERRIDE
        {
            throw std::logic_error( "Sections nested within threaded sections are not currently supported" );
        }

        virtual void sectionEnded( SectionEndInfo const& ) CATCH_OVERRIDE {
            throw std::logic_error( "Sections nested within threaded sections are not currently supported" );
        }

        virtual void sectionEndedEarly( SectionEndInfo const& ) CATCH_OVERRIDE {
            throw std::logic_error( "Sections nested within threaded sections are not currently supported" );
        }

        virtual void pushScopedMessage( MessageInfo const& message ) CATCH_OVERRIDE {
            m_messages.push_back( message );
        }

        virtual void popScopedMessage( MessageInfo const& message ) CATCH_OVERRIDE {
            m_messages.erase( std::remove( m_messages.begin(), m_messages.end(), message ), m_messages.end() );
        }

        virtual std::string getCurrentTestName() const CATCH_OVERRIDE {
            return m_parentContext.getCurrentTestName();
        }

        virtual AssertionResult const* getLastResult() const CATCH_OVERRIDE {
            return m_assertions.empty()
                ? CATCH_NULL
                : &m_assertions.back();
        }
        virtual IConfig const& config() const CATCH_OVERRIDE {
            return m_parentContext.config();
        }

        virtual void handleFatalErrorCondition( std::string const& message ) CATCH_OVERRIDE {
            m_parentContext.handleFatalErrorCondition( message );
        }

    public:
        virtual bool isAborting() const CATCH_OVERRIDE {
            // Probably can't do this
            CATCH_NOT_IMPLEMENTED;
        }
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_THREAD_CONTEXT_HPP_INCLUDED
