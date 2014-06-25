from  __future__ import  print_function

import os
import sys
import subprocess
import re

from scriptCommon import catchPath

rootPath = os.path.join( catchPath, 'projects/SelfTest/Baselines' )

filenameParser = re.compile( r'.*/(.*\..pp:)(.*)' )
filelineParser = re.compile( r'(.*\..pp:)([0-9]*)(.*)' )
lineNumberParser = re.compile( r'(.*)line="[0-9]*"(.*)' )
hexParser = re.compile( r'(.*)\b(0[xX][0-9a-fA-F]+)\b(.*)' )
durationsParser = re.compile( r'(.*)time="[0-9]*\.[0-9]*"(.*)' )
versionParser = re.compile( r'(.*?)Catch v[0-9]*.[0-9]* b[0-9]*(.*)' )

if len(sys.argv) == 2:
	cmdPath = sys.argv[1]
else:
	cmdPath = os.path.join( catchPath, 'projects/XCode/CatchSelfTest/DerivedData/CatchSelfTest/Build/Products/Debug/CatchSelfTest' )

overallResult = 0

def filterLine( line ):
	m = filenameParser.match( line )
	if m:
		line = m.group(1) + m.group(2)
		m = filelineParser.match( line )
		if m:
			line = m.group(1) + "<line number>" + m.group(3)
	else:
		m = lineNumberParser.match( line )
		if m:
			line = m.group(1) + m.group(2)
	m = versionParser.match( line )
	if m:
		line = m.group(1) + "<version>" + m.group(2)

	while True:
		m = hexParser.match( line )
		if m:
			line = m.group(1) + "0x<hex digits>" + m.group(3)
		else:
			break
	m = durationsParser.match( line )
	if m:
		line = m.group(1) + 'time="{duration}"' + m.group(2)
	return line

def approve( baseName, args ):
	global overallResult
	args[0:0] = [cmdPath]
	baselinesPath = os.path.join( rootPath, '{0}.approved.txt'.format( baseName ) )
	rawResultsPath = os.path.join( rootPath, '_{0}.tmp'.format( baseName ) )
	filteredResultsPath = os.path.join( rootPath, '{0}.unapproved.txt'.format( baseName ) )

	f = open( rawResultsPath, 'w' )
	subprocess.call( args, stdout=f, stderr=f )
	f.close()

	rawFile = open( rawResultsPath, 'r' )
	filteredFile = open( filteredResultsPath, 'w' )
	for line in rawFile:
		filteredFile.write( filterLine( line ).rstrip() + "\n" )
	filteredFile.close()
	rawFile.close()

	os.remove( rawResultsPath )
	print()
	print( baseName + ":" )
	if os.path.exists( baselinesPath ):
		diffResult = subprocess.call([ "diff", baselinesPath, filteredResultsPath ] )
		if diffResult == 0:
			os.remove( filteredResultsPath )
			print( "  \033[92mResults matched" )
		else:
			print( "  \n****************************\n  \033[91mResults differed" )
			if diffResult > overallResult:
				overallResult = diffResult
		print( "\033[0m" )
	else:
		print( "  first approval" )
		if overallResult == 0:
			overallResult = 1

# Standard console reporter
approve( "console.std", ["~_"] )
# console reporter, include passes, warn about No Assertions
approve( "console.sw", ["~_", "-s", "-w", "NoAssertions"] )
# console reporter, include passes, warn about No Assertions, limit failures to first 4
approve( "console.swa4", ["~_", "-s", "-w", "NoAssertions", "-x", "4"] )
# junit reporter, include passes, warn about No Assertions
approve( "junit.sw", ["~_", "-s", "-w", "NoAssertions", "-r", "junit"] )
# xml reporter, include passes, warn about No Assertions
approve( "xml.sw", ["~_", "-s", "-w", "NoAssertions", "-r", "xml"] )

if overallResult != 0:
	print( "run approve.py to approve new baselines" )
exit( overallResult)
