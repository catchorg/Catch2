from  __future__ import print_function

import os
import sys
import subprocess
import re

import scriptCommon
from scriptCommon import catchPath

rootPath = os.path.join(catchPath, 'projects/SelfTest/Baselines')

filelocParser = re.compile(r'.*/(.+\.[ch]pp:)([0-9]*)')
lineNumberParser = re.compile(r' line="[0-9]*"')
hexParser = re.compile(r'\b(0[xX][0-9a-fA-F]+)\b')
durationsParser = re.compile(r' time="[0-9]*\.[0-9]*"')
timestampsParser = re.compile(r' timestamp="\d{4}-\d{2}-\d{2}T\d{2}\:\d{2}\:\d{2}Z"')
versionParser = re.compile(r'Catch v[0-9]+\.[0-9]+\.[0-9]+(-develop\.[0-9]+)?')
nullParser = re.compile(r'\b(__null|nullptr)\b')
exeNameParser = re.compile(r'\b(CatchSelfTest|SelfTest)\b')

if len(sys.argv) == 2:
    cmdPath = sys.argv[1]
else:
    cmdPath = scriptCommon.getBuildExecutable()

if not cmdPath.startswith("/"):
    cmdPath = os.path.join(catchPath, cmdPath)

overallResult = 0

def filterLine(line):
    # make paths relative to Catch root
    line = line.replace(catchPath + '/', '')

    # strip source line numbers
    m = filelocParser.match(line)
    if m:
        # note that this also strips directories, leaving only the filename
        filename, lnum = m.groups()
        lnum = "<line number>" if lnum else ""
        line = filename + lnum + line[m.end():]
    else:
        line = lineNumberParser.sub(" ", line)

    # strip Catch version number
    line = versionParser.sub("<version>", line)

    # replace *null* with 0
    line = nullParser.sub("0", line)

    # strip executable name
    line = exeNameParser.sub("<exe-name>", line)

    # strip hexadecimal numbers (presumably pointer values)
    line = hexParser.sub("0x<hex digits>", line)

    # strip durations and timestamps
    line = durationsParser.sub(' time="{duration}"', line)
    line = timestampsParser.sub(' timestamp="{iso8601-timestamp}"', line)
    return line


def approve(baseName, args):
    global overallResult
    args[0:0] = [cmdPath]
    if not os.path.exists(cmdPath):
        raise Exception("Executable doesn't exist at " + cmdPath)
    baselinesPath = os.path.join(rootPath, '{0}.approved.txt'.format(baseName))
    rawResultsPath = os.path.join(rootPath, '_{0}.tmp'.format(baseName))
    filteredResultsPath = os.path.join(rootPath, '{0}.unapproved.txt'.format(baseName))

    f = open(rawResultsPath, 'w')
    subprocess.call(args, stdout=f, stderr=f)
    f.close()

    rawFile = open(rawResultsPath, 'r')
    filteredFile = open(filteredResultsPath, 'w')
    for line in rawFile:
        filteredFile.write(filterLine(line).rstrip() + "\n")
    filteredFile.close()
    rawFile.close()

    os.remove(rawResultsPath)
    print()
    print(baseName + ":")
    if os.path.exists(baselinesPath):
        diffResult = subprocess.call(["diff", baselinesPath, filteredResultsPath])
        if diffResult == 0:
            os.remove(filteredResultsPath)
            print("  \033[92mResults matched")
        else:
            print("  \n****************************\n  \033[91mResults differed")
            if diffResult > overallResult:
                overallResult = diffResult
        print("\033[0m")
    else:
        print("  first approval")
        if overallResult == 0:
            overallResult = 1


print("Running approvals against executable:")
print("  " + cmdPath)

# Standard console reporter
approve("console.std", ["~[c++11]", "--order", "lex"])
# console reporter, include passes, warn about No Assertions
approve("console.sw", ["~[c++11]", "-s", "-w", "NoAssertions", "--order", "lex"])
# console reporter, include passes, warn about No Assertions, limit failures to first 4
approve("console.swa4", ["~[c++11]", "-s", "-w", "NoAssertions", "-x", "4", "--order", "lex"])
# junit reporter, include passes, warn about No Assertions
approve("junit.sw", ["~[c++11]", "-s", "-w", "NoAssertions", "-r", "junit", "--order", "lex"])
# xml reporter, include passes, warn about No Assertions
approve("xml.sw", ["~[c++11]", "-s", "-w", "NoAssertions", "-r", "xml", "--order", "lex"])

if overallResult != 0:
    print("If these differenecs are expected run approve.py to approve new baselines")
exit(overallResult)
