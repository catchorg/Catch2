import os
import sys
import re
import datetime
import string

from scriptCommon import catchPath

versionParser = re.compile( r'(\s*const\sT\sLibraryVersionInfo<T>::value)\s*\(\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*\"(.*)\"\s*\).*' )
includesParser = re.compile( r'\s*#include\s*"(.*)"' )
guardParser = re.compile( r'\s*#.*TWOBLUECUBES_CATCH_.*_INCLUDED')
defineParser = re.compile( r'\s*#define')
ifParser = re.compile( r'\s*#ifndef TWOBLUECUBES_CATCH_.*_INCLUDED')
endIfParser = re.compile( r'\s*#endif // TWOBLUECUBES_CATCH_.*_INCLUDED')
ifImplParser = re.compile( r'\s*#if.*(CATCH_CONFIG_MAIN|CATCH_CONFIG_RUNNER)')
commentParser1 = re.compile( r'^\s*/\*')
commentParser2 = re.compile( r'^\s*\*')
blankParser = re.compile( r'^\s*$')
seenHeaders = set([])
rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.hpp" )
readmePath = os.path.join( catchPath, "README.md" )
outputPath = os.path.join( catchPath, 'single_include/catch.hpp' )

bumpVersion = True
includeImpl = True

for arg in sys.argv[1:]:
    arg = string.lower(arg)
    if arg == "nobump":
        bumpVersion = False
        print( "Not bumping version number" )
    elif arg == "noimpl":
        includeImpl = False
        bumpVersion = False
        print "Not including impl code (and not bumping version)"
    else:
        print "\n** Unrecognised argument: " + arg + " **\n"
        exit(1)

out = open( outputPath, 'w' )
ifdefs = 0
implIfDefs = -1

def write( line ):
    if includeImpl or implIfDefs == -1:
        out.write( line )

def parseFile( path, filename ):
    global ifdefs
    global implIfDefs

    f = open( path + filename, 'r' )
    blanks = 0
    for line in f:
        if ifParser.match( line ):
            ifdefs = ifdefs + 1
        elif endIfParser.match( line ):
            ifdefs = ifdefs - 1
        m = includesParser.match( line )
        if m:
            header = m.group(1)
            headerPath, sep, headerFile = header.rpartition( "/" )
            if not headerFile in seenHeaders:
                if headerFile != "tbc_text_format.h" and headerFile != "clara.h":
                    seenHeaders.add( headerFile )
                write( "// #included from: {0}\n".format( header ) )
                if( headerPath == "internal" and path.endswith( "internal/" ) ):
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
                if blanks < 2:
                    write( line.rstrip() + "\n" )

class Version:
    def __init__(self):
        f = open( versionPath, 'r' )
        for line in f:
            m = versionParser.match( line )
            if m:
                self.variableDecl = m.group(1)
                self.majorVersion = int(m.group(2))
                self.minorVersion = int(m.group(3))
                self.buildNumber = int(m.group(4))
                self.branchName = m.group(5)
        f.close()

    def incrementBuildNumber(self):
        self.buildNumber = self.buildNumber+1

    def updateVersionFile(self):
        f = open( versionPath, 'r' )
        lines = []
        for line in f:
            m = versionParser.match( line )
            if m:
                lines.append( '{0}( {1}, {2}, {3}, "{4}" );'.format( self.variableDecl, self.majorVersion, self.minorVersion, self.buildNumber, self.branchName ) )
            else:
                lines.append( line.rstrip() )
        f.close()
        f = open( versionPath, 'w' )
        for line in lines:
            f.write( line + "\n" )

    def updateReadmeFile(self):
        f = open( readmePath, 'r' )
        lines = []
        for line in f:
            lines.append( line.rstrip() )
        f.close()
        f = open( readmePath, 'w' )
        for line in lines:
            if line.startswith( "*v" ):
                f.write( '*v{0}.{1} build {2} ({3} branch)*\n'.format( self.majorVersion, self.minorVersion, self.buildNumber, self.branchName ) )
            else:
                f.write( line + "\n" )

def generateSingleInclude():
    v = Version()
    if bumpVersion:
        v.incrementBuildNumber()
        v.updateVersionFile()
        v.updateReadmeFile()
    out.write( "/*\n" )
    out.write( " *  CATCH v{0}.{1} build {2} ({3} branch)\n".format( v.majorVersion, v.minorVersion, v.buildNumber, v.branchName ) )
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

generateSingleInclude()
