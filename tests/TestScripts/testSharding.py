#!/usr/bin/env python3

"""
This test script verifies that sharding tests does change which tests are run.
This is done by running the binary multiple times, once to list all the tests,
once per shard to list the tests for that shard, and once again per shard to
execute the tests. The sharded lists are compared to the full list to ensure
none are skipped, duplicated, and that the order remains the same. This process
is repeated for multiple command line argument combinations to ensure sharding
works with different filters and test orderings.
"""

import subprocess
import sys
import xml.etree.ElementTree as ET

from collections import namedtuple

def make_base_commandline(self_test_exe):
    return [
        self_test_exe,
        '--reporter', 'xml',
        "--shard-count", "5",
        "--shard-index", "2",
        "[generators]~[benchmarks]~[.]"
    ]

def list_tests(self_test_exe):
    cmd = make_base_commandline(self_test_exe) + ['--list-tests']

    process = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr:
        raise RuntimeError("Unexpected error output:\n" + process.stderr)

    root = ET.fromstring(stdout)
    result = [elem.text for elem in root.findall('./TestCase/Name')]

    if len(result) < 2:
        raise RuntimeError("Unexpectedly few tests listed (got {})".format(
            len(result)))

    return result


def execute_tests(self_test_exe):
    cmd = make_base_commandline(self_test_exe)

    process = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr:
        raise RuntimeError("Unexpected error output:\n" + process.stderr)

    root = ET.fromstring(stdout)
    result = [elem.attrib["name"] for elem in root.findall('./TestCase')]

    if len(result) < 2:
        raise RuntimeError("Unexpectedly few tests listed (got {})".format(
            len(result)))
    return result


def check_listed_and_executed_tests_match(listed_tests, executed_tests):
        listed_names = set(listed_tests)
        executed_names = set(executed_tests)

        listed_string = "\n".join(listed_names)
        exeucted_string = "\n".join(executed_names)

        assert listed_names == executed_names, (
            "Executed tests do not match the listed tests:\nExecuted:\n{}\n\nListed:\n{}".format(exeucted_string, listed_string)
        )


def test_sharding(self_test_exe):
    listed_tests = list_tests(self_test_exe)
    executed_tests = execute_tests(self_test_exe)

    check_listed_and_executed_tests_match(listed_tests, executed_tests)


def main():
    self_test_exe, = sys.argv[1:]

    test_sharding(self_test_exe)

if __name__ == '__main__':
    sys.exit(main())
