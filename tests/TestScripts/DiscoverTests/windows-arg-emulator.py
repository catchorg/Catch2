#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

"""Emulate Windows command-line parsing when running under Wine.

Catch2 built for Windows treats a leading '/' as an option prefix. When a
Windows test executable is run under Wine, a bare '/'-rooted path following
'--out' is therefore parsed as an option, and discovery fails. This wrapper
reproduces that failure mode on any host, so the CMake helper tests can guard
against regressions without requiring Wine.
"""

import subprocess
import sys


def main():
    args = sys.argv[1:]
    for i in range(len(args) - 1):
        if args[i] == '--out' and args[i + 1].startswith('/'):
            sys.stderr.write(
                'Error(s) in input:\n  Expected argument following --out\n'
            )
            return 1
    return subprocess.call(args)


if __name__ == '__main__':
    sys.exit(main())
