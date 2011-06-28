import os
import sys
import re

includesParser = re.compile( r'\s*#include\s*"(.*)"' )
guardParser = re.compile( r'\s*#.*_INCLUDED')
defineParser = re.compile( r'\s*#define')
seenHeaders = set([])
rootPath = os.path.join( os.path.realpath(os.path.dirname(sys.argv[0])), 'include/' )

def parseFile( path, filename ):
	f = open( path + filename, 'r' )
	for line in f:
		m = includesParser.match( line )
		if m:
			header = m.group(1)
			headerPath, sep, headerFile = header.rpartition( "/" )
			if not headerFile in seenHeaders:
				seenHeaders.add( headerFile )
				print "// #included from: " + header
				print
				if( headerPath == "internal" and path.endswith( "internal/" ) ):
					headerPath = ""
					sep = ""
				if os.path.exists( path + headerPath + sep + headerFile ):
					parseFile( path + headerPath + sep, headerFile )
				else:
					parseFile( rootPath + headerPath + sep, headerFile )
		elif not guardParser.match( line ):
			print line.rstrip()
		elif defineParser.match( line ):
			print line.rstrip()
			

print "// This file has been merged from multiple headers. Please don't edit it directly"
print
print '#ifndef TWOBLUECUBES_CATCH_HPP_INCLUDED'
print '#define TWOBLUECUBES_CATCH_HPP_INCLUDED'
parseFile( rootPath, 'catch.hpp' )
print '#endif // TWOBLUECUBES_CATCH_HPP_INCLUDED'
print
