#!/usr/bin/env python3

import subprocess
import sys

def run_tests_with_threshold(self_test_exe, threshold):
    cmd = [self_test_exe, '--min-duration', str(threshold),
            '[min_duration_test]']
    process = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    if stderr:
        raise RuntimeError("Unexpected error output:\n" +
                stderr.decode())
    if process.returncode != 0:
        raise RuntimeError("Unexpected failure to run tests\n")
    result = stdout.split(b'\n')
    report_lines = [s.split() for s in result if b' s: ' in s]
    tests_reported = [l[2] for l in report_lines]
    times_reported = [float(l[0]) for l in report_lines]
    return tests_reported, times_reported

def check_times_at_least(times_reported, minimum):
    for time in times_reported:
        assert time >= minimum, (
                'Time {} was less that requested minimum {}' .format(
                    time, minimum))

def main():
    self_test_exe, = sys.argv[1:]
    tests, times = run_tests_with_threshold(self_test_exe, '0.15')
    assert tests == [b'sleep_for_200ms'], (
            "Unexpected tests reported %s" % tests)
    check_times_at_least(times, 0.15)
    tests,times = run_tests_with_threshold(self_test_exe, '0')
    assert tests == [b'sleep_for_100ms', b'sleep_for_200ms'], (
            "Unexpected tests reported %s" % tests)
    check_times_at_least(times, 0)

if __name__ == '__main__':
    sys.exit(main())
