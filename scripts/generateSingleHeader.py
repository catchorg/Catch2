#!/usr/bin/env python

from __future__ import print_function

import os
import io
import sys
import re
import datetime
from glob import glob

from scriptCommon import catchPath

def generate(v):
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
    outputPath = os.path.join( catchPath, 'single_include/catch2/catch.hpp' )

    globals = {
        'includeImpl' : True,
        'ifdefs'      :  0,
        'implIfDefs'  : -1
    }

    for arg in sys.argv[1:]:
        arg = arg.lower()
        if arg == "noimpl":
            globals['includeImpl'] = False
            print( "Not including impl code" )
        else:
            print( "\n** Unrecognised argument: " + arg + " **\n" )
            exit(1)


    # ensure that the output directory exists (hopefully no races)
    outDir = os.path.dirname(outputPath)
    if not os.path.exists(outDir):
        os.makedirs(outDir)
    out = io.open( outputPath, 'w', newline='\n')

    def write( line ):
        if globals['includeImpl'] or globals['implIfDefs'] == -1:
            out.write( line )

    def insertCpps():
        dirs = [os.path.join( rootPath, s) for s in ['', 'internal', 'reporters']]
        cppFiles = []
        for dir in dirs:
            cppFiles += glob(os.path.join(dir, '*.cpp'))
        # To minimize random diffs, sort the files before processing them
        for fname in sorted(cppFiles):
            dir, name = fname.rsplit(os.path.sep, 1)
            dir += os.path.sep
            parseFile(dir, name)

    def parseFile( path, filename ):
        f = io.open( os.path.join(path, filename), 'r', encoding='utf-8' )
        blanks = 0
        write( u"// start {0}\n".format( filename ) )
        for line in f:
            if '// ~*~* CATCH_CPP_STITCH_PLACE *~*~' in line:
                insertCpps()
                continue
            elif ifParser.match( line ):
                globals['ifdefs'] += 1
            elif endIfParser.match( line ):
                globals['ifdefs'] -= 1
                if globals['ifdefs'] == globals['implIfDefs']:
                    globals['implIfDefs'] = -1
            m = includesParser.match( line )
            if m:
                header = m.group(1)
                headerPath, sep, headerFile = header.rpartition( "/" )
                if headerFile not in seenHeaders:
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
                    globals['implIfDefs'] = globals['ifdefs']
                if (not guardParser.match( line ) or defineParser.match( line ) ) and not commentParser1.match( line )and not commentParser2.match( line ):
                    if blankParser.match( line ):
                        blanks = blanks + 1
                    else:
                        blanks = 0
                    if blanks < 2 and not defineParser.match(line):
                        write( line.rstrip() + "\n" )
        write( u'// end {}\n'.format(filename) )


    write( u"/*\n" )
    write( u" *  Catch v{0}\n".format( v.getVersionString() ) )
    write( u" *  Generated: {0}\n".format( datetime.datetime.now() ) )
    write( u" *  ----------------------------------------------------------\n" )
    write( u" *  This file has been merged from multiple headers. Please don't edit it directly\n" )
    write( u" *  Copyright (c) {} Two Blue Cubes Ltd. All rights reserved.\n".format( datetime.date.today().year ) )
    write( u" *\n" )
    write( u" *  Distributed under the Boost Software License, Version 1.0. (See accompanying\n" )
    write( u" *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)\n" )
    write( u" */\n" )
    write( u"#ifndef TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n" )
    write( u"#define TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n" )

    parseFile( rootPath, 'catch.hpp' )

    write( u"#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n\n" )
    out.close()
    print ("Generated single include for Catch v{0}\n".format( v.getVersionString() ) )


if __name__ == '__main__':
    from releaseCommon import Version
    generate(Version())
