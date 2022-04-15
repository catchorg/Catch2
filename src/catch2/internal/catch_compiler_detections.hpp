
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

#ifndef CATCH_COMPILER_DETECTIONS_HPP_INCLUDED
#define CATCH_COMPILER_DETECTIONS_HPP_INCLUDED

#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC) && !defined(__CUDACC__) && !defined(__LCC__)
#define CATCH_COMPILER_GCC
#endif

#if defined(__clang__)
#define CATCH_COMPILER_CLANG
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define CATCH_COMPILER_MSC
#endif

#if defined(__MINGW32__)
#define CATCH_COMPILER_MINGW32
#endif

#endif // CATCH_COMPILER_DETECTIONS_HPP_INCLUDED