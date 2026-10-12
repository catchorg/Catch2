#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

import argparse
import copy
import json
import os
import re
import statistics
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TEMPLATE = os.path.join(HERE, "listing_template.json")
SHIM = os.path.join(HERE, "copy_shim.cmake")
COUNTS = [1, 10, 500, 1000, 2000, 4000, 8000, 16000]
#COUNTS = [1, 10, 100]
REPEATS = 5

def load_template(path: str) -> dict:
    with open(path, encoding="utf-8") as f:
        catch_out = json.load(f)
    if 'listings' not in catch_out or 'tests' not in catch_out['listings']:
        sys.exit(f"Template '{path}' does not contain the expected 'listings.tests' data")

    return catch_out


def synthesize_listing(template_doc: dict, count: int, file, with_tags: bool) -> str:
    """Writes JSON listing with exactly `count` test cases into `file`.

    The template entries are cycled through, and each name is made unique by
    appending an index, so that every registered CTest test has a distinct name.
    """
    # To avoid messing up the template tests by deletion, we have
    # to take a deepcopy before the changes.
    doc_copy = copy.deepcopy(template_doc)
    template_tests = doc_copy['listings']['tests']
    if not with_tags:
        for test in template_tests:
            del test['tags']
    num_original = len(template_tests)

    out_tests = []
    for i in range(count):
        entry = template_tests[i % num_original].copy()
        entry["name"] = f"{entry['name']} #{i // num_original}"
        out_tests.append(entry)

    doc_copy['listings']['tests'] = out_tests

    # indent=2 matches Catch2's pretty printing
    json.dump(doc_copy, file, indent=2)
    file.write('\n')


def build_command(args, listing_path: str, ctest_file: str, add_tags: bool) -> list[str]:
    # The executor needs to be a CMake list, so it has to be separated by ';'
    executor = ";".join(['cmake', f"-DBENCH_LISTING={listing_path}", "-P", SHIM, "--"])
    return [
        'cmake',
        "-DTEST_TARGET=benchmark",
        f"-DTEST_EXECUTABLE={listing_path}",
        f"-DTEST_EXECUTOR={executor}",
        f"-DTEST_WORKING_DIR={os.path.dirname(ctest_file)}",
        f"-DCTEST_FILE={ctest_file}",
        f"-DADD_TAGS_AS_LABELS={'TRUE' if add_tags else 'FALSE'}",
        "-P",
        args.script,
    ]


def run_once(cmd: list[str]) -> float:
    start = time.monotonic()
    result = subprocess.run(cmd, capture_output=True, text=True)
    elapsed = time.monotonic() - start
    if result.returncode != 0:
        sys.stderr.write("Benchmark invocation failed:\n")
        sys.stderr.write(f"  cmd: {' '.join(cmd)}\n")
        sys.stderr.write(f"  stdout: {result.stdout}\n")
        sys.stderr.write(f"  stderr: {result.stderr}\n")
        sys.exit(1)
    return elapsed


def verify_output(ctest_file: str, count: int, add_tags: bool):
    """Sanity-check the generated CTest script and return its add_test count."""
    with open(ctest_file, encoding="utf-8") as f:
        content = f.read()
    add_test_lines = sum(1 for line in content.splitlines() if line.startswith("add_test("))
    if add_test_lines != count:
        sys.exit(
            f"Sanity check failed: expected {count} add_test() lines, "
            f"found {add_test_lines} in {ctest_file}"
        )
    if add_tags and count > 0 and "LABELS" not in content:
        sys.exit(
            f"Sanity check failed: ADD_TAGS_AS_LABELS was on but no LABELS found "
            f"in {ctest_file}"
        )


def cmake_version() -> str:
    stdout = subprocess.run(
        ['cmake', "--version"], capture_output=True, text=True, check=True
    ).stdout
    match = re.match("cmake version (.+)", stdout)
    return match.group(1)


def print_table(counts, tag_modes, results):
    timing_by_ct = {(count, tags): median_time for (count, tags, median_time) in results}

    headers = ["N"]
    if False in tag_modes:
        headers.append("tags:off")
    if True in tag_modes:
        headers.append("tags:on")

    def fmt_cell(count, tags_on):
        median_time = timing_by_ct[(count, tags_on)]
        return f"{median_time:8.1f} ms"

    # Header
    row = f"{headers[0]:>6}"
    if False in tag_modes:
        row += f"  {headers[1]:>14}"
    if True in tag_modes:
        row += f"  {headers[2]:>14}"
 
    print()
    print("-" * len(row))
    print(row)
    print("-" * len(row))

    for count in counts:
        row = f"{count:>6}"
        if False in tag_modes:
            row += f"  {fmt_cell(count, False):>14}"
        if True in tag_modes:
            row += f"  {fmt_cell(count, True):>14}"
        print(row)


def main():
    parser = argparse.ArgumentParser(
        description="Benchmark the CMake-side cost of catch_discover_tests."
    )
    parser.add_argument(
        "--script",
        required=True,
        help="Path to the CatchAddTests.cmake to benchmark.",
    )
    args = parser.parse_args()

    if not os.path.exists(args.script):
        sys.exit(f"Script to benchmark '{args.script}' does not exist")
    if not os.path.exists(SHIM):
        sys.exit(f"Copy-shim '{SHIM}' does not exist")

    tag_modes = [False, True]

    template_doc = load_template(TEMPLATE)

    print(f"Benchmarking:  {os.path.abspath(args.script)}")
    print(f"CMake version: {cmake_version()}")
    print(f"Ns:            {COUNTS}")
    print(f"Repeats:       {REPEATS}")
    print()


    results = []  # (count, tags_on, median_ms)
    with tempfile.TemporaryDirectory() as workdir:
        os.makedirs(workdir, exist_ok=True)
        for count in COUNTS:
            listing_path = os.path.join(workdir, f"listing-{count}.json")

            for add_tags in tag_modes:
                with open(listing_path, "w", encoding="utf-8") as f:
                    synthesize_listing(template_doc, count, f, add_tags)
                mode = "on" if add_tags else "off"
                ctest_file = os.path.join(workdir, f"ctest-{count}-{mode}.cmake")
                cmd = build_command(args, listing_path, ctest_file, add_tags)

                # Untimed warmup run (populates any filesystem/OS caches).
                run_once(cmd)
                verify_output(ctest_file, count, add_tags)

                timings = [run_once(cmd) for _ in range(REPEATS)]
                median_ms = statistics.median(timings) * 1000.0
                results.append((count, add_tags, median_ms))
            print(f'N = {count} done in ~{len(tag_modes) * sum(timings):.2f} s')

    print_table(COUNTS, tag_modes, results)


if __name__ == "__main__":
    main()
