#!/bin/bash

# make sure single-file header was regenerated
../scripts/generateSingleHeader.py

# run Test case with specified section to trigger bug #1394
../cmake-build-debug/examples/300-FilteredSection \#1394 -c RunSection
../cmake-build-debug/examples/300-FilteredSection \#1394\ nested -c NestedRunSection -c s1
