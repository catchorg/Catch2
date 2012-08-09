import os
import sys
import re
import datetime

includesParser = re.compile( r'\s*#include\s*"(.*)"' )
guardParser = re.compile( r'\s*#.*_INCLUDED')
defineParser = re.compile( r'\s*#define')
commentParser1 = re.compile( r'^\s*/\*')
commentParser2 = re.compile( r'^\s*\*')
blankParser = re.compile( r'^\s*$')
seenHeaders = set([])
rootPath = os.path.join( os.path.realpath(os.path.dirname(sys.argv[0])), 'include/' )

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

print "/*"
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