#!/usr/bin/env python

from __future__ import print_function

import os
import re
import urllib2
import json

from scriptCommon import catchPath
from scriptCommon import runAndCapture

issueNumberRe = re.compile( r'(.*?)#([0-9]*)([^0-9]?.*)' )

rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.hpp" )


hashes = runAndCapture( ['git', 'log', '-2', '--format="%H"', versionPath] )
lines = runAndCapture( ['git', 'log', hashes[1] + ".." + hashes[0], catchPath] )

prevLine = ""
messages = []
dates = []
issues = {}

def getIssueTitle( issueNumber ):
    try:
        s = urllib2.urlopen("https://api.github.com/repos/philsquared/catch/issues/" + issueNumber ).read()        
    except:
        return "#HTTP Error#"

    try:
        j = json.loads( s )
        return j["title"]
    except:
        return "#JSON Error#"

for line in lines:
    if line.startswith( "commit"):
        pass
    elif line.startswith( "Author:"):
        pass
    elif line.startswith( "Date:"):
        dates.append( line[5:].lstrip() )
    elif line == "" and prevLine == "":
        pass
    else:
        prevLine = line
        match = issueNumberRe.match( line )
        line2 = ""
        while match:
            issueNumber = match.group(2)
            issue = '#{0} ("{1}")'.format( issueNumber, getIssueTitle( issueNumber ) )
            line2 = line2 + match.group(1) + issue
            match = issueNumberRe.match( match.group(3) )
        if line2 == "":
            messages.append( line )
        else:
            messages.append( line2 )

print("All changes between {0} and {1}:\n".format( dates[-1], dates[0] ))

for line in messages:
    print(line)
