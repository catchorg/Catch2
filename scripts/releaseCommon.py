from  __future__ import  print_function

import os
import sys
import re
import string

from scriptCommon import catchPath

versionParser = re.compile( r'(\s*static\sVersion\sversion)\s*\(\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*\"(.*)\"\s*,\s*(.*)\s*\).*' )
rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.cpp" )
definePath = os.path.join(rootPath, 'catch.hpp')
readmePath = os.path.join( catchPath, "README.md" )
conanPath = os.path.join(catchPath, 'conanfile.py')
conanTestPath = os.path.join(catchPath, 'test_package', 'conanfile.py')
cmakePath = os.path.join(catchPath, 'CMakeLists.txt')

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

def updateReadmeFile(version):
    import updateWandbox

    downloadParser = re.compile( r'<a href=\"https://github.com/catchorg/Catch2/releases/download/v\d+\.\d+\.\d+/catch.hpp\">' )
    success, wandboxLink = updateWandbox.uploadFiles()
    if not success:
        print('Error when uploading to wandbox: {}'.format(wandboxLink))
        exit(1)
    f = open( readmePath, 'r' )
    lines = []
    for line in f:
        lines.append( line.rstrip() )
    f.close()
    f = open( readmePath, 'w' )
    for line in lines:
        line = downloadParser.sub( r'<a href="https://github.com/catchorg/Catch2/releases/download/v{0}/catch.hpp">'.format(version.getVersionString()) , line)
        if '[![Try online](https://img.shields.io/badge/try-online-blue.svg)]' in line:
            line = '[![Try online](https://img.shields.io/badge/try-online-blue.svg)]({0})'.format(wandboxLink)
        f.write( line + "\n" )

def updateConanFile(version):
    conanParser = re.compile( r'    version = "\d+\.\d+\.\d+.*"')
    f = open( conanPath, 'r' )
    lines = []
    for line in f:
        m = conanParser.match( line )
        if m:
            lines.append( '    version = "{0}"'.format(format(version.getVersionString())) )
        else:
            lines.append( line.rstrip() )
    f.close()
    f = open( conanPath, 'w' )
    for line in lines:
        f.write( line + "\n" )

def updateConanTestFile(version):
    conanParser = re.compile( r'    requires = \"Catch\/\d+\.\d+\.\d+.*@%s\/%s\" % \(username, channel\)')
    f = open( conanTestPath, 'r' )
    lines = []
    for line in f:
        m = conanParser.match( line )
        if m:
            lines.append( '    requires = "Catch/{0}@%s/%s" % (username, channel)'.format(format(version.getVersionString())) )
        else:
            lines.append( line.rstrip() )
    f.close()
    f = open( conanTestPath, 'w' )
    for line in lines:
        f.write( line + "\n" )

def updateCmakeFile(version):
    with open(cmakePath, 'r') as file:
        lines = file.readlines()
    with open(cmakePath, 'w') as file:
        for line in lines:
            if 'project(Catch2 LANGUAGES CXX VERSION ' in line:
                file.write('project(Catch2 LANGUAGES CXX VERSION {0})\n'.format(version.getVersionString()))
            else:
                file.write(line)


def updateVersionDefine(version):
    with open(definePath, 'r') as file:
        lines = file.readlines()
    with open(definePath, 'w') as file:
        for line in lines:
            if '#define CATCH_VERSION_MAJOR' in line:
                file.write('#define CATCH_VERSION_MAJOR {}\n'.format(version.majorVersion))
            elif '#define CATCH_VERSION_MINOR' in line:
                file.write('#define CATCH_VERSION_MINOR {}\n'.format(version.minorVersion))
            elif '#define CATCH_VERSION_PATCH' in line:
                file.write('#define CATCH_VERSION_PATCH {}\n'.format(version.patchNumber))
            else:
                file.write(line)


def performUpdates(version):
    # First update version file, so we can regenerate single header and
    # have it ready for upload to wandbox, when updating readme
    version.updateVersionFile()
    updateVersionDefine(version)

    import generateSingleHeader
    generateSingleHeader.generate(version)

    # Then copy the reporters to single include folder to keep them in sync
    # We probably should have some kind of convention to select which reporters need to be copied automagically,
    # but this works for now
    import shutil
    for rep in ('automake', 'tap', 'teamcity'):
        sourceFile = os.path.join(catchPath, 'include/reporters/catch_reporter_{}.hpp'.format(rep))
        destFile = os.path.join(catchPath, 'single_include', 'catch2', 'catch_reporter_{}.hpp'.format(rep))
        shutil.copyfile(sourceFile, destFile)

    updateReadmeFile(version)
    updateConanFile(version)
    updateConanTestFile(version)
    updateCmakeFile(version)
