#!/usr/bin/env python

from __future__ import print_function

import os
import sys
import re
import datetime
import string

from scriptCommon import catchPath


def generate(v):
    includesParser = re.compile( r'\s*#\s*include\s*"(.*)"' )
    guardParser = re.compile( r'\s*#.*TWOBLUECUBES_CATCH_.*_INCLUDED')
    defineParser = re.compile( r'\s*#define')
    ifParser = re.compile( r'\s*#ifndef TWOBLUECUBES_CATCH_.*_INCLUDED')
    endIfParser = re.compile( r'\s*#endif // TWOBLUECUBES_CATCH_.*_INCLUDED')
    ifImplParser = re.compile( r'\s*#ifdef CATCH_CONFIG_RUNNER' )
    commentParser1 = re.compile( r'^\s*/\*')
    commentParser2 = re.compile( r'^ \*')
    blankParser = re.compile( r'^\s*$')
    seenHeaders = set([])
    rootPath = os.path.join( catchPath, 'include/' )
    outputPath = os.path.join( catchPath, 'single_include/catch.hpp' )


    globals = {
        'ifdefs'     :  0,
        'implIfDefs' : -1,
        'includeImpl': True
    }

    for arg in sys.argv[1:]:
        arg = string.lower(arg)
        if arg == "noimpl":
            globals['includeImpl'] = False
            print( "Not including impl code" )
        else:
            print( "\n** Unrecognised argument: " + arg + " **\n" )
            exit(1)

    out = open( outputPath, 'w' )

    def write( line ):
        if globals['includeImpl'] or globals['implIfDefs'] == -1:
            out.write( line )

    def parseFile( path, filename ):
        f = open( path + filename, 'r' )
        blanks = 0
        for line in f:
            if ifParser.match( line ):
                globals['ifdefs'] += 1
            elif endIfParser.match( line ):
                globals['ifdefs'] -= 1
                if globals['ifdefs'] == globals['implIfDefs']:
                    globals['implIfDefs'] = -1
            m = includesParser.match( line )
            if m:
                header = m.group(1)
                headerPath, sep, headerFile = header.rpartition( "/" )
                if not headerFile in seenHeaders:
                    if headerFile != "tbc_text_format.h" and headerFile != "clara.h":
                        seenHeaders.add( headerFile )
                    write( "// #included from: {0}\n".format( header ) )
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
                    if blanks < 2:
                        write( line.rstrip() + "\n" )


    out.write( "/*\n" )
    out.write( " *  Catch v{0}\n".format( v.getVersionString() ) )
    out.write( " *  Generated: {0}\n".format( datetime.datetime.now() ) )
    out.write( " *  ----------------------------------------------------------\n" )
    out.write( " *  This file has been merged from multiple headers. Please don't edit it directly\n" )
    out.write( " *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.\n" )
    out.write( " *\n" )
    out.write( " *  Distributed under the Boost Software License, Version 1.0. (See accompanying\n" )
    out.write( " *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)\n" )
    out.write( " */\n" )
    out.write( "#ifndef TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n" )
    out.write( "#define TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n" )

    parseFile( rootPath, 'catch.hpp' )

    out.write( "#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED\n\n" )
    out.close()
    print ("Generated single include for Catch v{0}\n".format( v.getVersionString() ) )

if __name__ == '__main__':
    from releaseCommon import Version
    generate(Version())
