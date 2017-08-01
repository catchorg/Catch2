#!/usr/bin/env python

from __future__ import print_function

import os
import sys
import re
import datetime
import string
from glob import glob

from scriptCommon import catchPath
from releaseCommon import Version


includesParser = re.compile( r'\s*#\s*include\s*"(.*)"' )
guardParser = re.compile( r'\s*#.*(TWOBLUECUBES_)?CATCH_.*_INCLUDED')
defineParser = re.compile( r'\s*#define\s+(TWOBLUECUBES_)?CATCH_.*_INCLUDED')
ifParser = re.compile( r'\s*#ifndef (TWOBLUECUBES_)?CATCH_.*_INCLUDED')
endIfParser = re.compile( r'\s*#endif // (TWOBLUECUBES_)?CATCH_.*_INCLUDED')
ifImplParser = re.compile( r'\s*#ifdef CATCH_CONFIG_RUNNER' )
commentParser1 = re.compile( r'^\s*/\*')
commentParser2 = re.compile( r'^ \*')
blankParser = re.compile( r'^\s*$')

seenHeaders = set([])
rootPath = os.path.join( catchPath, 'include/' )
outputPath = os.path.join( catchPath, 'single_include/catch.hpp' )

includeImpl = True

for arg in sys.argv[1:]:
    arg = string.lower(arg)
    if arg == "noimpl":
        includeImpl = False
        print( "Not including impl code" )
    else:
        print( "\n** Unrecognised argument: " + arg + " **\n" )
        exit(1)


# ensure that the output directory exists (hopefully no races)
outDir = os.path.dirname(outputPath)
if not os.path.exists(outDir):
    os.makedirs(outDir)
out = open( outputPath, 'w' )
ifdefs = 0
implIfDefs = -1

def write( line ):
    if includeImpl or implIfDefs == -1:
        out.write( line )

def insertCpps():
    dirs = [os.path.join( rootPath, s) for s in ['', 'internal', 'reporters']]
    cppFiles = []
    for dir in dirs:
        cppFiles += glob(os.path.join(dir, '*.cpp'))
    for fname in cppFiles:
        dir, name = fname.rsplit(os.path.sep, 1)
        dir += os.path.sep
        parseFile(dir, name)

def parseFile( path, filename ):
    global ifdefs
    global implIfDefs

    f = open( os.path.join(path, filename), 'r' )
    blanks = 0
    write( "// start {0}\n".format( filename ) )
    for line in f:
        if '// ~*~* CATCH_CPP_STITCH_PLACE *~*~' in line:
            insertCpps()
        elif ifParser.match( line ):
            ifdefs = ifdefs + 1
        elif endIfParser.match( line ):
            ifdefs = ifdefs - 1
            if ifdefs == implIfDefs:
                implIfDefs = -1
        m = includesParser.match( line )
        if m:
            header = m.group(1)
            headerPath, sep, headerFile = header.rpartition( "/" )
            if not headerFile in seenHeaders:
                if headerFile != "tbc_text_format.h" and headerFile != "clara.h":
                    seenHeaders.add( headerFile )
                if headerPath == "internal" and path.endswith("internal/"):
                    headerPath = ""
                    sep = ""
                if os.path.exists( path + headerPath + sep + headerFile ):
                    parseFile( path + headerPath + sep, headerFile )
                else:
                    parseFile( rootPath + headerPath + sep, headerFile )
        else:
            if ifImplParser.match(line):
                implIfDefs = ifdefs
            if (not guardParser.match( line ) or defineParser.match( line ) ) and not commentParser1.match( line )and not commentParser2.match( line ):
                if blankParser.match( line ):
                    blanks = blanks + 1
                else:
                    blanks = 0
                if blanks < 2 and not defineParser.match(line):
                    write( line.rstrip() + "\n" )
    write( '// end {}\n'.format(filename) )


v = Version()
out.write( "/*\n" )
out.write( " *  Catch v{0}\n".format( v.getVersionString() ) )
out.write( " *  Generated: {0}\n".format( datetime.datetime.now() ) )
out.write( " *  ----------------------------------------------------------\n" )
out.write( " *  This file has been merged from multiple headers. Please don't edit it directly\n" )
out.write( " *  Copyright (c) {} Two Blue Cubes Ltd. All rights reserved.\n".format( datetime.date.today().year ) )
out.write( " *\n" )
out.write( " *  Distributed under the Boost Software License, Version 1.0. (See accompanying\n" )
out.write( " *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)\n" )
out.write( " */\n" )
out.write( "#ifndef TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n" )
out.write( "#define TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n" )

parseFile( rootPath, 'catch.hpp' )

out.write( "#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n\n" )

print ("Generated single include for Catch v{0}\n".format( v.getVersionString() ) )
