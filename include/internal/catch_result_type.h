/*
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_TYPE_H_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_TYPE_H_INCLUDED

namespace Catch {

    // ResultWas::OfType enum
    struct ResultWas { enum OfType {
        Unknown = -1,
        Ok = 0,
        Info = 1,
        Warning = 2,

        FailureBit = 0x10,

        ExpressionFailed = FailureBit | 1,
        ExplicitFailure = FailureBit | 2,

        Exception = 0x100 | FailureBit,

        ThrewException = Exception | 1,
        DidntThrowException = Exception | 2

    }; };

    inline bool isOk( ResultWas::OfType resultType ) {
        return ( resultType & ResultWas::FailureBit ) == 0;
    }
    inline bool isJustInfo( int flags ) {
        return flags == ResultWas::Info;
    }


    // ResultDisposition::Flags enum
    struct ResultDisposition { enum Flags {
        Normal = 0x00,

        ContinueOnFailure = 0x01,   // Failures fail test, but execution continues
        FalseTest = 0x02,           // Prefix expression with !
        SuppressFail = 0x04         // Failures are reported but do not fail the test
    }; };

    inline ResultDisposition::Flags operator | ( ResultDisposition::Flags lhs, ResultDisposition::Flags rhs ) {
        return static_cast<ResultDisposition::Flags>( static_cast<int>( lhs ) | static_cast<int>( rhs ) );
    }

    inline bool shouldContinueOnFailure( int flags )    { return ( flags & ResultDisposition::ContinueOnFailure ) != 0; }
    inline bool isFalseTest( int flags )                { return ( flags & ResultDisposition::FalseTest ) != 0; }
    inline bool shouldSuppressFailure( int flags )      { return ( flags & ResultDisposition::SuppressFail ) != 0; }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_TYPE_H_INCLUDED
