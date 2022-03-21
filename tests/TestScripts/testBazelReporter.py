#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE_1_0.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

import os
import re
import sys
import xml.etree.ElementTree as ET
import subprocess

"""
Tests the CMake configure option for CATCH_CONFIG_DEFAULT_REPORTER

Requires 2 arguments, path to where the output files should be stored
and the name of the test
"""
if len(sys.argv) != 3:
    print("Wrong number of arguments: {}".format(len(sys.argv)))
    print("Usage: {} bin-path bin-name".format(sys.argv[0]))
    exit(1)


bin_path = os.path.abspath(sys.argv[1])
bin_name = sys.argv[2]
xml_out_path = os.path.join(bin_path, "{}.xml".format(bin_name))
config_path = "Debug" if os.name == "nt" else ""

# Ensure no file exists from previous test runs
if os.path.isfile(xml_out_path):
    os.remove(xml_out_path)

args = [os.path.join(bin_path, config_path, bin_name)]
env = os.environ.copy()
env["XML_OUTPUT_FILE"] = xml_out_path
test_passing = True

try:
    ret = subprocess.run(
        args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
        universal_newlines=True,
	env=env
    )
    stdout = ret.stdout
except subprocess.SubprocessError as ex:
    if ex.returncode == 1:
        # The test cases are allowed to fail.
        test_passing = False
        stdout = ex.stdout
    else:
        print('Could not run "{}"'.format(args))
        print("Return code: {}".format(ex.returncode))
        print("stdout: {}".format(ex.stdout))
        print("stderr: {}".format(ex.stdout))
        raise

# Check for valid XML output
try:
    tree = ET.parse(xml_out_path)
except ET.ParseError as ex:
    print("Invalid XML: '{}'".format(ex))
    raise
except FileNotFoundError as ex:
    print("Could not find '{}'".format(xml_out_path))
    raise

# Check for matching testsuite
if not tree.find('.//testsuite[@name="{}"]'.format(bin_name)):
    print("Could not find '{}' testsuite".format(bin_name))
    exit(2)

summary_test_cases = re.findall(r'test cases: \d* \| \d* passed \| \d* failed', stdout)
if len(summary_test_cases) == 0:
    print("Could not find test summary in {}".format(stdout))
    exit(2)

total, passed, failed = [int(s) for s in summary_test_cases[0].split() if s.isdigit()]

if failed == 0 and not test_passing:
    print("Expected at least 1 test failure!")
    exit(2)

if len(tree.findall('.//testcase')) != total:
    print("Unexpected number of test cases!")
    exit(2)

if len(tree.findall('.//failure')) != failed:
    print("Unexpected number of test failures!")
    exit(2)

if (passed + failed) != total:
    print("Something has gone very wrong, ({} + {}) != {}".format(passed, failed, total))
    exit(2)
