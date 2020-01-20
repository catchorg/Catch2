/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_HPP_INCLUDED

#include <catch2/catch_version_macros.hpp>

#include <catch2/catch_suppress_warnings.h>

#if !defined(CATCH_CONFIG_IMPL_ONLY)
#include <catch2/catch_platform.h>

#include <catch2/catch_user_interfaces.h>
#include <catch2/catch_tag_alias_autoregistrar.h>
#include <catch2/catch_test_registry.h>
#include <catch2/catch_capture.hpp>
#include <catch2/catch_section.h>
#include <catch2/catch_interfaces_exception.h>
#include <catch2/catch_approx.h>
#include <catch2/catch_compiler_capabilities.h>
#include <catch2/catch_string_manip.h>

#ifndef CATCH_CONFIG_DISABLE_MATCHERS
#include <catch2/catch_capture_matchers.h>
#endif
#include <catch2/catch_generators.hpp>
#include <catch2/catch_generators_generic.hpp>
#include <catch2/catch_generators_specific.hpp>

// These files are included here so the single_include script doesn't put them
// in the conditionally compiled sections
#include <catch2/catch_test_case_info.h>
#include <catch2/catch_interfaces_runner.h>

#ifdef __OBJC__
#include <catch2/catch_objc.hpp>
#endif

#include <catch2/catch_external_interfaces.h>

#if defined(CATCH_CONFIG_ENABLE_BENCHMARKING)
#include <catch2/benchmark/catch_benchmark.hpp>
#endif

#endif // ! CATCH_CONFIG_IMPL_ONLY

#if !defined(CATCH_CONFIG_IMPL_ONLY)

#ifdef CLARA_CONFIG_MAIN_NOT_DEFINED
#  undef CLARA_CONFIG_MAIN
#endif

#include <catch2/catch_test_macros.hpp>

#endif // ! CATCH_CONFIG_IMPL_ONLY

#include <catch2/catch_reenable_warnings.h>

using Catch::Detail::Approx;

#endif // TWOBLUECUBES_CATCH_HPP_INCLUDED
