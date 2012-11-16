import os
import sys
import re
import datetime

versionParser = re.compile( r'(\s*Version\slibraryVersion\s*=)\s*{\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*\"(.*)\"\s*}.*' )
includesParser = re.compile( r'\s*#include\s*"(.*)"' )
guardParser = re.compile( r'\s*#.*_INCLUDED')
defineParser = re.compile( r'\s*#define')
commentParser1 = re.compile( r'^\s*/\*')
commentParser2 = re.compile( r'^\s*\*')
blankParser = re.compile( r'^\s*$')
seenHeaders = set([])
catchPath = os.path.realpath(os.path.dirname(sys.argv[0]))
rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.hpp" )
#outputPath = os.path.join( catchPath, 'single_include/catch.hpp' )

def parseFile( path, filename ):
	f = open( path + filename, 'r' )
	blanks = 0
	for line in f:
		m = includesParser.match( line )
		if m:
			header = m.group(1)
			headerPath, sep, headerFile = header.rpartition( "/" )
			if not headerFile in seenHeaders:
				seenHeaders.add( headerFile )
				print "// #included from: " + header
				if( headerPath == "internal" and path.endswith( "internal/" ) ):
					headerPath = ""
					sep = ""
				if os.path.exists( path + headerPath + sep + headerFile ):
					parseFile( path + headerPath + sep, headerFile )
				else:
					parseFile( rootPath + headerPath + sep, headerFile )
		elif (not guardParser.match( line ) or defineParser.match( line ) ) and not commentParser1.match( line )and not commentParser2.match( line ):
			if blankParser.match( line ):
				blanks = blanks + 1
			else:
				blanks = 0
			if blanks < 2:
				print line.rstrip()

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
				lines.append( '{0} {{ {1}, {2}, {3}, "{4}" }};'.format( self.variableDecl, self.majorVersion, self.minorVersion, self.buildNumber, self.branchName ) )
			else:
				lines.append( line.rstrip() )
		f.close()
		f = open( versionPath, 'w' )
		for line in lines:
			f.write( line + "\n" )

def generateSingleInclude():
	v = Version()
	v.incrementBuildNumber()
	v.updateVersionFile()
	print "/*"
	print " *  CATCH v{0}.{1} build {2} ({3} branch)".format( v.majorVersion, v.minorVersion, v.buildNumber, v.branchName )
	print " *  Generated: " + str( datetime.datetime.now() )
	print " *  ----------------------------------------------------------"
	print " *  This file has been merged from multiple headers. Please don't edit it directly"
	print " *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved."
	print " *"
	print " *  Distributed under the Boost Software License, Version 1.0. (See accompanying"
	print " *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)"
	print " */"
	print '#ifndef TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED'
	print '#define TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED'
	parseFile( rootPath, 'catch.hpp' )
	print '#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED'
	print

generateSingleInclude()
