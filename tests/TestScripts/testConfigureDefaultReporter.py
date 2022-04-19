#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE_1_0.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

from ConfigureTestsCommon import configure_and_build, run_and_return_output

import os
import re
import sys

"""
Tests the CMake configure option for CATCH_CONFIG_DEFAULT_REPORTER

Requires 2 arguments, path folder where the Catch2's main CMakeLists.txt
exists, and path to where the output files should be stored.
"""

if len(sys.argv) != 3:
    print('Wrong number of arguments: {}'.format(len(sys.argv)))
    print('Usage: {} catch2-top-level-dir base-build-output-dir'.format(sys.argv[0]))
    exit(1)

catch2_source_path = os.path.abspath(sys.argv[1])
build_dir_path = os.path.join(os.path.abspath(sys.argv[2]), 'CMakeConfigTests', 'DefaultReporter')

configure_and_build(catch2_source_path,
                    build_dir_path,
                    [("CATCH_CONFIG_DEFAULT_REPORTER", "compact")])

stdout, _ = run_and_return_output(os.path.join(build_dir_path, 'tests'), 'SelfTest', ['[approx][custom]'])


# This matches the summary line made by compact reporter, console reporter's
# summary line does not match the regex.
summary_regex = 'Passed \d+ test case with \d+ assertions.'
if not re.search(summary_regex, stdout):
    print("Could not find '{}' in the stdout".format(summary_regex))
    print('stdout: "{}"'.format(stdout))
    exit(2)
