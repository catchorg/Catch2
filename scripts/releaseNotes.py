import os

from scriptCommon import catchPath
from scriptCommon import runAndCapture

rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.hpp" )


hashes = runAndCapture( ['git', 'log', '-2', '--format="%H"', versionPath] )
lines = runAndCapture( ['git', 'log', hashes[1] + ".." + hashes[0], catchPath] )

prevLine = ""
messages = []
dates = []
for line in lines:
    if line.startswith( "commit"):
        pass
    elif line.startswith( "Author:"):
        pass
    elif line.startswith( "Date:"):
        dates.append( line[5:].lstrip() )
        pass
    elif line == "" and prevLine == "":
        pass
    else:
        messages.append( line )
        prevLine = line

print "All changes between {0} and {1}:\n".format( dates[-1], dates[0] )

for line in messages:
    print line
