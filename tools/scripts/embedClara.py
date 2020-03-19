#!/usr/bin/env python3

# Execute this script any time you import a new copy of Clara into the third_party area
import os
import sys
import embed

rootPath = os.path.dirname(os.path.realpath( os.path.dirname(sys.argv[0])))

filename = os.path.join( rootPath, "third_party", "clara.hpp" )
outfilename = os.path.join( rootPath, "include", "external", "clara.hpp" )


# Mapping of pre-processor identifiers
idMap = {
   "CLARA_HPP_INCLUDED": "CATCH_CLARA_HPP_INCLUDED",
    "CLARA_CONFIG_CONSOLE_WIDTH": "CATCH_CLARA_CONFIG_CONSOLE_WIDTH",
    "CLARA_TEXTFLOW_HPP_INCLUDED": "CATCH_CLARA_TEXTFLOW_HPP_INCLUDED",
    "CLARA_TEXTFLOW_CONFIG_CONSOLE_WIDTH": "CATCH_CLARA_TEXTFLOW_CONFIG_CONSOLE_WIDTH",
    "CLARA_PLATFORM_WINDOWS": "CATCH_PLATFORM_WINDOWS"
    }

# outer namespace to add
outerNamespace = { "clara": ("Catch", "clara") }

mapper = embed.LineMapper( idMap, outerNamespace )
mapper.mapFile( filename, outfilename )