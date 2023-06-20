#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

import os
import subprocess
import sys


def build_project(sources_dir, output_base_path, catch2_path):
    build_dir = os.path.join(output_base_path, 'ctest-registration-test')
    config_cmd = ['cmake',
                  '-B', build_dir,
                  '-S', sources_dir,
                  f'-DCATCH2_PATH={catch2_path}',
                  '-DCMAKE_BUILD_TYPE=Debug']

    build_cmd = ['cmake',
                 '--build', build_dir,
                 '--config', 'Debug']

    try:
        subprocess.run(config_cmd,
                       capture_output = True,
                       check = True,
                       text = True)
        subprocess.run(build_cmd,
                       capture_output = True,
                       check = True,
                       text = True)
    except subprocess.CalledProcessError as err:
        print('Error when building the test project')
        print(f'cmd: {err.cmd}')
        print(f'stderr: {err.stderr}')
        print(f'stdout: {err.stdout}')
        exit(3)

    return build_dir



def get_test_names(build_path):
    # For now we assume that Windows builds are done using MSBuild under
    # Debug configuration. This means that we need to add "Debug" folder
    # to the path when constructing it. On Linux, we don't add anything.
    config_path = "Debug" if os.name == 'nt' else ""
    full_path = os.path.join(build_path, config_path, 'tests')


    cmd = [full_path, '--reporter', 'xml', '--list-tests']
    result = subprocess.run(cmd,
                            capture_output = True,
                            check = True,
                            text = True)

    import xml.etree.ElementTree as ET
    root = ET.fromstring(result.stdout)
    return [tc.text for tc in root.findall('TestCase/Name')]


def list_ctest_tests(build_path):
    old_path = os.getcwd()
    os.chdir(build_path)

    cmd = ['ctest', '-C', 'debug', '--show-only=json-v1']
    result = subprocess.run(cmd,
                            capture_output = True,
                            check = True,
                            text = True)
    os.chdir(old_path)

    import json

    ctest_response = json.loads(result.stdout)
    tests = ctest_response['tests']
    test_names = []
    for test in tests:
        test_command = test['command']
        # First part of the command is the binary, second is the filter.
        # If there are less, registration has failed. If there are more,
        # registration has changed and the script needs updating.
        assert len(test_command) == 2
        test_names.append(test_command[1])
        test_name = test_command[1]

    return test_names

def escape_catch2_test_name(name):
    for char in ('\\', ',', '[', ']'):
        name = name.replace(char, f"\\{char}")
    return name

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} path-to-catch2-cml output-path')
        exit(2)
    catch2_path = sys.argv[1]
    output_base_path = sys.argv[2]
    sources_dir = os.path.dirname(os.path.abspath(sys.argv[0]))

    build_path = build_project(sources_dir, output_base_path, catch2_path)

    catch_test_names = [escape_catch2_test_name(name) for name in get_test_names(build_path)]
    ctest_test_names = list_ctest_tests(build_path)

    mismatched = 0
    for catch_test in catch_test_names:
        if catch_test not in ctest_test_names:
            print(f"Catch2 test '{catch_test}' not found in CTest")
            mismatched += 1
    for ctest_test in ctest_test_names:
        if ctest_test not in catch_test_names:
            print(f"CTest test '{ctest_test}' not found in Catch2")
            mismatched += 1

    if mismatched:
        print(f"Found {mismatched} mismatched tests catch test names and ctest test commands!")
        exit(1)
