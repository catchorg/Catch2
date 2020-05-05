/** \file
 * This is a convenience header for Catch2's Reporter support. It includes
 * **all** of Catch2 headers related to reporters, including all reporters.
 *
 * Generally the Catch2 users should use specific includes they need,
 * but this header can be used instead for ease-of-experimentation, or
 * just plain convenience, at the cost of (significantly) increased
 * compilation times.
 *
 * When a new header (reporter) is added to either the `reporter` folder,
 * or to the corresponding internal subfolder, it should be added here.
 */

#ifndef CATCH_REPORTERS_ALL_HPP_INCLUDED
#define CATCH_REPORTERS_ALL_HPP_INCLUDED

#include <catch2/reporters/catch_reporter_automake.hpp>
#include <catch2/reporters/catch_reporter_bases.hpp>
#include <catch2/reporters/catch_reporter_compact.hpp>
#include <catch2/reporters/catch_reporter_console.hpp>
#include <catch2/reporters/catch_reporter_junit.hpp>
#include <catch2/reporters/catch_reporter_listening.hpp>
#include <catch2/reporters/catch_reporter_sonarqube.hpp>
#include <catch2/reporters/catch_reporter_tap.hpp>
#include <catch2/reporters/catch_reporter_teamcity.hpp>
#include <catch2/reporters/catch_reporter_xml.hpp>

#endif // CATCH_REPORTERS_ALL_HPP_INCLUDED
