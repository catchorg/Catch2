import os
import sys
import subprocess
import re

filenameParser = re.compile( r'\s*.*/(.*\.cpp)(.*)' )
hexParser = re.compile( r'(.*)\b(0[xX][0-9a-fA-F]+)\b(.*)' )

catchPath = os.path.realpath(os.path.dirname(sys.argv[0]))
baselinesPath = os.path.join( catchPath, 'projects/SelfTest/Baselines/approvedResults.txt' )
rawResultsPath = os.path.join( catchPath, 'projects/SelfTest/Baselines/_rawResults.tmp' )
filteredResultsPath = os.path.join( catchPath, 'projects/SelfTest/Baselines/unapprovedResults.txt' )

cmdPath = sys.argv[1]

f = open( rawResultsPath, 'w' )
subprocess.call([ cmdPath, "~dummy", "-s", "-w", "NoAssertions", "-r", "basic" ], stdout=f, stderr=f )
subprocess.call([ cmdPath, "~dummy", "-s", "-w", "NoAssertions", "-r", "basic", "-a", "4" ], stdout=f, stderr=f )
#subprocess.call([ cmdPath, "~dummy", "-s", "-w", "NoAssertions", "-r", "console" ], stdout=f, stderr=f )
subprocess.call([ cmdPath, "~dummy", "-s", "-w", "NoAssertions", "-r", "junit" ], stdout=f, stderr=f )
subprocess.call([ cmdPath, "~dummy", "-s", "-w", "NoAssertions", "-r", "xml" ], stdout=f, stderr=f )
f.close()

rawFile = open( rawResultsPath, 'r' )
filteredFile = open( filteredResultsPath, 'w' )
for line in rawFile:
	m = filenameParser.match( line )
	if m:
		line = m.group(1) + m.group(2)

	while True:
		m = hexParser.match( line )
		if m:
			line = m.group(1) + "0x<hex digits>" + m.group(3)
		else:
			break
	filteredFile.write( line.rstrip() + "\n" )
filteredFile.close()
rawFile.close()

os.remove( rawResultsPath )
print
diffResult = subprocess.call([ "diff", baselinesPath, filteredResultsPath ] )
if diffResult == 0:
	os.remove( filteredResultsPath )
	print "\033[92mResults matched"
else:
	print "\n****************************\n\033[91mResults differed"
print "\033[0m"
exit( diffResult)