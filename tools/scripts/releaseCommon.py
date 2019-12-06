from  __future__ import  print_function

import os
import sys
import re
import string
import glob
import fnmatch

from scriptCommon import catchPath

versionParser = re.compile( r'(\s*static\sVersion\sversion)\s*\(\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*\"(.*)\"\s*,\s*(.*)\s*\).*' )
rootPath = os.path.join( catchPath, 'include/' )
versionPath = os.path.join( rootPath, "internal/catch_version.cpp" )
definePath = os.path.join(rootPath, 'catch.hpp')
readmePath = os.path.join( catchPath, "README.md" )
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


def updateCmakeFile(version):
    with open(cmakePath, 'rb') as file:
        lines = file.readlines()
    replacementRegex = re.compile(b'project\\(Catch2 LANGUAGES CXX VERSION \\d+\\.\\d+\\.\\d+\\)')
    replacement = 'project(Catch2 LANGUAGES CXX VERSION {0})'.format(version.getVersionString()).encode('ascii')
    with open(cmakePath, 'wb') as file:
        for line in lines:
            file.write(replacementRegex.sub(replacement, line))


def updateVersionDefine(version):
    # First member of the tuple is the compiled regex object, the second is replacement if it matches
    replacementRegexes = [(re.compile(b'#define CATCH_VERSION_MAJOR \\d+'),'#define CATCH_VERSION_MAJOR {}'.format(version.majorVersion).encode('ascii')),
                          (re.compile(b'#define CATCH_VERSION_MINOR \\d+'),'#define CATCH_VERSION_MINOR {}'.format(version.minorVersion).encode('ascii')),
                          (re.compile(b'#define CATCH_VERSION_PATCH \\d+'),'#define CATCH_VERSION_PATCH {}'.format(version.patchNumber).encode('ascii')),
                         ]
    with open(definePath, 'rb') as file:
        lines = file.readlines()
    with open(definePath, 'wb') as file:
        for line in lines:
            for replacement in replacementRegexes:
                line = replacement[0].sub(replacement[1], line)
            file.write(line)


def updateVersionPlaceholder(filename, version):
    with open(filename, 'rb') as file:
        lines = file.readlines()
    placeholderRegex = re.compile(b' in Catch X.Y.Z')
    replacement = ' in Catch {}.{}.{}'.format(version.majorVersion, version.minorVersion, version.patchNumber).encode('ascii')
    with open(filename, 'wb') as file:
        for line in lines:
            file.write(placeholderRegex.sub(replacement, line))


def updateDocumentationVersionPlaceholders(version):
    print('Updating version placeholder in documentation')
    docsPath = os.path.join(catchPath, 'docs/')
    for basePath, _, files in os.walk(docsPath):
        for file in files:
            if fnmatch.fnmatch(file, "*.md") and "contributing.md" != file:
                updateVersionPlaceholder(os.path.join(basePath, file), version)


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
    for rep in ('automake', 'tap', 'teamcity', 'sonarqube'):
        sourceFile = os.path.join(catchPath, 'include/reporters/catch_reporter_{}.hpp'.format(rep))
        destFile = os.path.join(catchPath, 'single_include', 'catch2', 'catch_reporter_{}.hpp'.format(rep))
        shutil.copyfile(sourceFile, destFile)

    updateReadmeFile(version)
    updateCmakeFile(version)
    updateDocumentationVersionPlaceholders(version)
