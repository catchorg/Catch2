
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/** \file
 * This is a convenience header for Catch2. It includes **all** of Catch2 headers.
 *
 * Generally the Catch2 users should use specific includes they need,
 * but this header can be used instead for ease-of-experimentation, or
 * just plain convenience, at the cost of (significantly) increased
 * compilation times.
 *
 * When a new header is added to either the top level folder, or to the
 * corresponding internal subfolder, it should be added here. Headers
 * added to the various subparts (e.g. matchers, generators, etc...),
 * should go their respective catch-all headers.
 */

#ifndef CATCH_ALL_HPP_INCLUDED
#define CATCH_ALL_HPP_INCLUDED

#include <catch2/benchmark/catch_benchmark_all.hpp>  // IWYU pragma: export
#include <catch2/catch_approx.hpp>  // IWYU pragma: export
#include <catch2/catch_assertion_info.hpp>  // IWYU pragma: export
#include <catch2/catch_assertion_result.hpp>  // IWYU pragma: export
#include <catch2/catch_config.hpp>  // IWYU pragma: export
#include <catch2/catch_get_random_seed.hpp>  // IWYU pragma: export
#include <catch2/catch_message.hpp>  // IWYU pragma: export
#include <catch2/catch_section_info.hpp>  // IWYU pragma: export
#include <catch2/catch_session.hpp>  // IWYU pragma: export
#include <catch2/catch_tag_alias.hpp>  // IWYU pragma: export
#include <catch2/catch_tag_alias_autoregistrar.hpp>  // IWYU pragma: export
#include <catch2/catch_template_test_macros.hpp>  // IWYU pragma: export
#include <catch2/catch_test_case_info.hpp>  // IWYU pragma: export
#include <catch2/catch_test_macros.hpp>  // IWYU pragma: export
#include <catch2/catch_test_spec.hpp>  // IWYU pragma: export
#include <catch2/catch_timer.hpp>  // IWYU pragma: export
#include <catch2/catch_tostring.hpp>  // IWYU pragma: export
#include <catch2/catch_totals.hpp>  // IWYU pragma: export
#include <catch2/catch_translate_exception.hpp>  // IWYU pragma: export
#include <catch2/catch_version.hpp>  // IWYU pragma: export
#include <catch2/catch_version_macros.hpp>  // IWYU pragma: export
#include <catch2/generators/catch_generators_all.hpp>  // IWYU pragma: export
#include <catch2/interfaces/catch_interfaces_all.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_assertion_handler.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_case_insensitive_comparisons.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_case_sensitive.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_clara.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_commandline.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_compare_traits.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_compiler_capabilities.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_config_android_logwrite.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_config_counter.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_config_prefix_messages.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_config_static_analysis_support.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_config_uncaught_exceptions.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_config_wchar.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_console_colour.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_console_width.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_container_nonmembers.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_context.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_debug_console.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_debugger.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_decomposer.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_enforce.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_enum_values_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_errno_guard.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_exception_translator_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_fatal_condition_handler.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_floating_point_helpers.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_getenv.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_is_permutation.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_istream.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_lazy_expr.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_leak_detector.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_list.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_logical_traits.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_message_info.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_meta.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_move_and_forward.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_noncopyable.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_optional.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_output_redirect.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_parse_numbers.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_platform.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_polyfills.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_preprocessor.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_preprocessor_internal_stringify.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_preprocessor_remove_parens.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_random_number_generator.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_random_seed_generation.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_reporter_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_reporter_spec_parser.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_result_type.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_reusable_string_stream.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_run_context.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_section.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_sharding.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_singletons.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_source_line_info.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_startup_exception_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_stdstreams.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_stream_end_stop.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_string_manip.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_stringref.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_tag_alias_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_template_test_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_case_info_hasher.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_case_registry_impl.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_case_tracker.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_failure_exception.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_macro_impl.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_registry.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_run_info.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_test_spec_parser.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_textflow.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_to_string.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_uncaught_exceptions.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_unique_name.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_unique_ptr.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_void_type.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_wildcard_pattern.hpp>  // IWYU pragma: export
#include <catch2/internal/catch_xmlwriter.hpp>  // IWYU pragma: export
#include <catch2/matchers/catch_matchers_all.hpp>  // IWYU pragma: export
#include <catch2/reporters/catch_reporters_all.hpp>  // IWYU pragma: export

#endif // CATCH_ALL_HPP_INCLUDED
