#!/usr/bin/env python3

from __future__ import print_function

import io
import os
import sys
import subprocess
import re
import difflib

import scriptCommon
from scriptCommon import catchPath

if os.name == 'nt':
    # Enable console colours on windows
    os.system('')

rootPath = os.path.join(catchPath, 'tests/SelfTest/Baselines')

langFilenameParser = re.compile(r'(.+\.[ch]pp)')
filelocParser = re.compile(r'''
    .*/
    (.+\.[ch]pp)  # filename
    (?::|\()      # : is starting separator between filename and line number on Linux, ( on Windows
    ([0-9]*)      # line number
    \)?           # Windows also has an ending separator, )
''', re.VERBOSE)
lineNumberParser = re.compile(r' line="[0-9]*"')
hexParser = re.compile(r'\b(0[xX][0-9a-fA-F]+)\b')
# Note: junit must serialize time with 3 (or or less) decimal places
#       before generalizing this parser, make sure that this is checked
#       in other places too.
junitDurationsParser = re.compile(r' time="[0-9]+\.[0-9]{3}"')
durationParser = re.compile(r''' duration=['"][0-9]+['"]''')
timestampsParser = re.compile(r'\d{4}-\d{2}-\d{2}T\d{2}\:\d{2}\:\d{2}Z')
versionParser = re.compile(r'Catch2 v[0-9]+\.[0-9]+\.[0-9]+(-\w*\.[0-9]+)?')
nullParser = re.compile(r'\b(__null|nullptr)\b')
exeNameParser = re.compile(r'''
    \b
    SelfTest                  # Expected executable name
    (?:.exe)?                 # Executable name contains .exe on Windows.
    \b
''', re.VERBOSE)
# This is a hack until something more reasonable is figured out
specialCaseParser = re.compile(r'file\((\d+)\)')

sinceEpochParser = re.compile(r'\d+ .+ since epoch')

# The weird OR is there to always have at least empty string for group 1
tapTestNumParser = re.compile(r'^((?:not ok)|(?:ok)|(?:warning)|(?:info)) (\d+) -')

if len(sys.argv) == 2:
    cmdPath = sys.argv[1]
else:
    cmdPath = os.path.join(catchPath, scriptCommon.getBuildExecutable())

overallResult = 0


def diffFiles(fileA, fileB):
    with io.open(fileA, 'r', encoding='utf-8', errors='surrogateescape') as file:
        aLines = [line.rstrip() for line in file.readlines()]
    with io.open(fileB, 'r', encoding='utf-8', errors='surrogateescape') as file:
        bLines = [line.rstrip() for line in file.readlines()]

    shortenedFilenameA = fileA.rsplit(os.sep, 1)[-1]
    shortenedFilenameB = fileB.rsplit(os.sep, 1)[-1]

    diff = difflib.unified_diff(aLines, bLines, fromfile=shortenedFilenameA, tofile=shortenedFilenameB, n=0)
    return [line for line in diff if line[0] in ('+', '-')]


def normalizeFilepath(line):
    # Sometimes the path separators used by compiler and Python can differ,
    # so we try to match the path with both forward and backward path
    # separators, to make the paths relative to Catch2 repo root.
    forwardSlashPath = catchPath.replace('\\', '/')
    if forwardSlashPath in line:
        line = line.replace(forwardSlashPath + '/', '')
    backwardSlashPath = catchPath.replace('/', '\\')
    if backwardSlashPath in line:
        line = line.replace(backwardSlashPath + '\\', '')

    m = langFilenameParser.match(line)
    if m:
        filepath = m.group(0)
        # go from \ in windows paths to /
        filepath = filepath.replace('\\', '/')
        # remove start of relative path
        filepath = filepath.replace('../', '')
        line = line[:m.start()] + filepath + line[m.end():]

    return line

def filterLine(line, isCompact):
    line = normalizeFilepath(line)

    # strip source line numbers
    m = filelocParser.match(line)
    if m:
        # note that this also strips directories, leaving only the filename
        filename, lnum = m.groups()
        lnum = ":<line number>" if lnum else ""
        line = filename + lnum + line[m.end():]
    else:
        line = lineNumberParser.sub(" ", line)

    if isCompact:
        line = line.replace(': FAILED', ': failed')
        line = line.replace(': PASSED', ': passed')

    # strip out the test order number in TAP to avoid massive diffs for every change
    line = tapTestNumParser.sub("\g<1> {test-number} -", line)

    # strip Catch2 version number
    line = versionParser.sub("<version>", line)

    # replace *null* with 0
    line = nullParser.sub("0", line)

    # strip executable name
    line = exeNameParser.sub("<exe-name>", line)

    # strip hexadecimal numbers (presumably pointer values)
    line = hexParser.sub("0x<hex digits>", line)

    # strip durations and timestamps
    line = junitDurationsParser.sub(' time="{duration}"', line)
    line = durationParser.sub(' duration="{duration}"', line)
    line = timestampsParser.sub('{iso8601-timestamp}', line)
    line = specialCaseParser.sub('file:\g<1>', line)
    line = sinceEpochParser.sub('{since-epoch-report}', line)
    return line


def get_rawResultsPath(baseName):
    return os.path.join(rootPath, '_{0}.tmp'.format(baseName))


def get_baselinesPath(baseName):
    return os.path.join(rootPath, '{0}.approved.txt'.format(baseName))


def get_filteredResultsPath(baseName):
    return os.path.join(rootPath, '{0}.unapproved.txt'.format(baseName))


def run_test(baseName, args):
    args[0:0] = [cmdPath]
    if not os.path.exists(cmdPath):
        raise Exception("Executable doesn't exist at " + cmdPath)

    print(args)
    rawResultsPath = get_rawResultsPath(baseName)
    f = open(rawResultsPath, 'w')
    subprocess.call(args, stdout=f, stderr=f)
    f.close()


def check_outputs(baseName):
    global overallResult
    rawResultsPath = get_rawResultsPath(baseName)
    baselinesPath = get_baselinesPath(baseName)
    filteredResultsPath = get_filteredResultsPath(baseName)

    rawFile = io.open(rawResultsPath, 'r', encoding='utf-8', errors='surrogateescape')
    filteredFile = io.open(filteredResultsPath, 'w', encoding='utf-8', errors='surrogateescape')
    for line in rawFile:
        filteredFile.write(filterLine(line, 'compact' in baseName).rstrip() + "\n")
    filteredFile.close()
    rawFile.close()

    os.remove(rawResultsPath)
    print()
    print(baseName + ":")
    if os.path.exists(baselinesPath):
        diffResult = diffFiles(baselinesPath, filteredResultsPath)
        if diffResult:
            print('\n'.join(diffResult))
            print("  \n****************************\n  \033[91mResults differed")
            if len(diffResult) > overallResult:
                overallResult = len(diffResult)
        else:
            os.remove(filteredResultsPath)
            print("  \033[92mResults matched")
        print("\033[0m")
    else:
        print("  first approval")
        if overallResult == 0:
            overallResult = 1


def approve(baseName, args):
    run_test(baseName, args)
    check_outputs(baseName)


print("Running approvals against executable:")
print("  " + cmdPath)


## special cases first:
# Standard console reporter
approve("console.std", ["~[!nonportable]~[!benchmark]~[approvals] *", "--order", "lex", "--rng-seed", "1"])
# console reporter, include passes, warn about No Assertions, limit failures to first 4
approve("console.swa4", ["~[!nonportable]~[!benchmark]~[approvals] *", "-s", "-w", "NoAssertions", "-x", "4", "--order", "lex", "--rng-seed", "1"])

## Common reporter checks: include passes, warn about No Assertions
reporters = ('console', 'junit', 'xml', 'compact', 'sonarqube', 'tap', 'teamcity', 'automake')
for reporter in reporters:
    filename = '{}.sw'.format(reporter)
    common_args = ["~[!nonportable]~[!benchmark]~[approvals] *", "-s", "-w", "NoAssertions", "--order", "lex", "--rng-seed", "1"]
    reporter_args = ['-r', reporter]
    approve(filename, common_args + reporter_args)

## All reporters at the same time

common_args = ["~[!nonportable]~[!benchmark]~[approvals] *", "-s", "-w", "NoAssertions", "--order", "lex", "--rng-seed", "1"]
filenames = ['{}.sw.multi'.format(reporter) for reporter in reporters]
reporter_args = []
for reporter, filename in zip(reporters, filenames):
    reporter_args += ['-r', '{}::out={}'.format(reporter, get_rawResultsPath(filename))]

run_test("default.sw.multi", common_args + reporter_args)
check_outputs("default.sw.multi")
for reporter, filename in zip(reporters, filenames):
    check_outputs(filename)


if overallResult != 0:
    print("If these differences are expected, run approve.py to approve new baselines.")
exit(overallResult)
