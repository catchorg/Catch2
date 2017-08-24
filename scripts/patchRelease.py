#!/usr/bin/env python

from  __future__ import  print_function
import releaseCommon

v = releaseCommon.Version()
v.incrementPatchNumber()
releaseCommon.performUpdates(v)

print( "Updated Version.hpp, README and Conan to v{0}".format( v.getVersionString() ) )
