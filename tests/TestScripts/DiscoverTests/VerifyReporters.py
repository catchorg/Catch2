#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

import pathlib
import subprocess
import sys
import xml.etree.ElementTree as ET


def run(command):
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        print(f'Command failed: {command}')
        print(result.stdout)
        print(result.stderr)
        result.check_returncode()
    return result.stdout


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} path-to-catch2-cml output-path')
        sys.exit(2)

    catch2_path = pathlib.Path(sys.argv[1]).resolve()
    build_path = pathlib.Path(sys.argv[2]).resolve() / 'ctest-reporter-test'
    sources = pathlib.Path(__file__).resolve().parent / 'Reporters'

    run(['cmake', '-S', str(sources), '-B', str(build_path),
         f'-DCATCH2_PATH={catch2_path}', '-DCMAKE_BUILD_TYPE=Debug'])
    run(['cmake', '--build', str(build_path), '--config', 'Debug'])

    reports = build_path / 'reporter output'
    for report in reports.rglob('*.xml'):
        report.unlink()

    for mode in ('PRE_TEST', 'POST_BUILD', 'file_only', 'stdout_only'):
        output = run(['ctest', '--test-dir', str(build_path), '-C', 'Debug',
                      '-V', '-R', f'^{mode}:'])
        has_console_output = 'All tests passed' in output
        has_colour = '\x1b[' in output
        expected_console = mode != 'file_only'
        assert has_console_output == expected_console, output
        assert has_colour == expected_console, output

        paths = sorted((reports / mode).glob('*.xml'))
        if mode == 'stdout_only':
            assert not paths, paths
            print(f'{mode}: console output verified')
            continue
        assert len(paths) == 2, paths
        names = []
        for path in paths:
            assert path.name.startswith('result-'), path
            test_cases = ET.parse(path).getroot().findall('.//testcase')
            assert len(test_cases) == 1, path.read_text()
            names.append(test_cases[0].attrib['name'])
        assert sorted(names) == ['first reporter case', 'second reporter case'], names
        print(f'{mode}: per-test JUnit files verified; console output: {expected_console}')
