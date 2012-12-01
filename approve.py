import os
import sys
import shutil

catchPath = os.path.realpath(os.path.dirname(sys.argv[0]))
baselinesPath = os.path.join( catchPath, 'projects/SelfTest/Baselines/approvedResults.txt' )
filteredResultsPath = os.path.join( catchPath, 'projects/SelfTest/Baselines/unapprovedResults.txt' )

if os.path.isfile( filteredResultsPath ):
	os.remove( baselinesPath )
	os.rename( filteredResultsPath, baselinesPath )
else:
	print "approval file " + filteredResultsPath + " does not exist"
