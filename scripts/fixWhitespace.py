#!/usr/bin/env python

from  __future__ import  print_function
import os
from scriptCommon import catchPath

changedFiles = 0

def isSourceFile( path ):
    return path.endswith( ".cpp" ) or path.endswith( ".h" ) or path.endswith( ".hpp" )

def fixAllFilesInDir( dir ):
    for f in os.listdir( dir ):
        path = os.path.join( dir,f )
        if os.path.isfile( path ):
            if isSourceFile( path ):
                fixFile( path )
        else:
            fixAllFilesInDir( path )

def fixFile( path ):
    f = open( path, 'r' )
    lines = []
    changed = 0
    for line in f:
        trimmed = line.rstrip() + "\n"
        trimmed = trimmed.replace('\t', '    ')
        if trimmed != line:
            changed = changed +1
        lines.append( trimmed )
    f.close()
    if changed > 0:
        global changedFiles
        changedFiles = changedFiles + 1
        print( path + ":" )
        print( " - fixed " + str(changed) + " line(s)" )
        altPath = path + ".backup"
        os.rename( path, altPath )
        f2 = open( path, 'w' )
        for line in lines:
            f2.write( line )
        f2.close()
        os.remove( altPath )

fixAllFilesInDir(catchPath)
if changedFiles > 0:
    print( "Fixed " + str(changedFiles) + " file(s)" )
else:
    print( "No trailing whitespace found" )
