# Catch on npm

This document describes how use to Catch with [node-gyp](https://github.com/nodejs/node-gyp) for [Node.js](https://github.com/nodejs/node) applications. Thanks to @philsquared and Catch contributors for the library, and @smikes for the foundation of the npm/node-gyp stuff!

# Using catch via npm

A `package.json` is provided for installation with [npm](https://github.com/npm/npm). NB: as of this writing (2016-09-21) the `catch` npm package refers to @smikes's original work which uses an older version of Catch. You can use `@bchociej/catch` for this version.


# Settings in binding.gyp

`include_dirs` should reference the Catch include directory:

    "include_dirs": [
    	"<!(node -e \"require('catch')\")"
    ]

Catch requires exceptions, so add these two lines to your target (for Unixy systems):

    "cflags!":    ["-fno-exceptions"],
    "cflags_cc!": ["-fno-exceptions"]

Caveat developer: I'm not 100% sure how to do the same on Windows or Mac. These are my estimations based on node's `common.gypi` file. You may need to alter them to fit your needs.

    # Windows
    "defines!": ["_HAS_EXCEPTIONS=0"]

    # Mac
	"xcode_settings": {"GCC_ENABLE_CPP_EXCEPTIONS": "YES"}

# Build the test executable on its own

You can build a standalone test executable with a separate node-gyp target, e.g.:

    {
    	"target_name": "test_executable_name",
    	"type":        "executable",
    	"cflags!":     ["-fno-exceptions"], # for Unixes; others see above
    	"cflags_cc!":  ["-fno-exceptions"], # for Unixes; others see above
    	"sources": [
    		"test/testfile.cc",
    		"test/testfile2.cc",
    		"src/source.cc",
    		"src/source2.cc",
    		(etc),
    	],
    	"include_dirs": [
    		"<!(node -e \"require('catch')\")"
    	]
    }

The test executable will end up at `build/Release/test_executable_name` or `build/Debug/test_executable_name` or so.

# Using Catch

     #define CATCH_CONFIG_MAIN
     #include "catch/single_include/catch.hpp"

Follow the [tutorial](https://github.com/philsquared/Catch/blob/master/docs/tutorial.md) for more
details.
