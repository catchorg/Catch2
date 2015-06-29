from  __future__ import  print_function

import os
import sys
import re
import string

from scriptCommon import catchPath

versionParser = re.compile( r'(\s*Version\slibraryVersion)\s*\(\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*\"(.*)\"\s*,\s*(.*)\s*\).*' )
rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.hpp" )
readmePath = os.path.join( catchPath, "README.md" )

class Version:
    def __init__(self):
        f = open( versionPath, 'r' )
        for line in f:
            m = versionParser.match( line )
            if m:
                self.variableDecl = m.group(1)
                self.majorVersion = int(m.group(2))
                self.minorVersion = int(m.group(3))
                self.patchNumber = int(m.group(4))
                self.branchName = m.group(5)
                self.buildNumber = int(m.group(6))
        f.close()

    def nonDevelopRelease(self):
        if self.branchName != "":
            self.branchName = ""
            self.buildNumber = 0
    def developBuild(self):
        if self.branchName == "":
            self.branchName = "develop"
            self.buildNumber = 0

    def incrementBuildNumber(self):
        self.developBuild()
        self.buildNumber = self.buildNumber+1

    def incrementPatchNumber(self):
        self.nonDevelopRelease()
        self.patchNumber = self.patchNumber+1

    def incrementMinorVersion(self):
        self.nonDevelopRelease()
        self.patchNumber = 0
        self.minorVersion = self.minorVersion+1

    def incrementMajorVersion(self):
        self.nonDevelopRelease()
        self.patchNumber = 0
        self.minorVersion = 0
        self.majorVersion = self.majorVersion+1

    def getVersionString(self):
        versionString = '{0}.{1}.{2}'.format( self.majorVersion, self.minorVersion, self.patchNumber )
        if self.branchName != "":
            versionString = versionString + '-{0}.{1}'.format( self.branchName, self.buildNumber )
        return versionString

    def updateVersionFile(self):
        f = open( versionPath, 'r' )
        lines = []
        for line in f:
            m = versionParser.match( line )
            if m:
                lines.append( '{0}( {1}, {2}, {3}, "{4}", {5} );'.format( self.variableDecl, self.majorVersion, self.minorVersion, self.patchNumber, self.branchName, self.buildNumber ) )
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
                f.write( '*v{0}*\n'.format( self.getVersionString() ) )
            else:
                f.write( line + "\n" )

