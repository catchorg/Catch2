#!/usr/bin/env python

from  __future__ import  print_function
from releaseCommon import Version

v = Version()
v.incrementMajorVersion()
v.updateVersionFile()
v.updateReadmeFile()
v.updateConanFile()
v.updateConanTestFile()

print( "Updated Version.hpp, README and Conan to v{0}".format( v.getVersionString() ) )
