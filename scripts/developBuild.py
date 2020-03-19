#!/usr/bin/env python3

from  __future__ import  print_function
import releaseCommon

v = releaseCommon.Version()
v.incrementBuildNumber()
releaseCommon.performUpdates(v)

print( "Updated Version.hpp, README and Conan to v{0}".format( v.getVersionString() ) )
