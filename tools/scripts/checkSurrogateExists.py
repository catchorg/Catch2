#!/bin/python3
import os
import sys

srcdir = "../../src/catch2"
surrogates_folder = "../../tests/SurrogateCpps"
header_files = []
surrogate_files = []
throw_error = False

def get_files(filetype,dir):
    filenames = []

    for _, _, files in os.walk(dir):
        for file in files:
            if filetype in file:
                filenames.append(file)

    return filenames

def check_surrogate_exists(headers,surrogates):
    global throw_error
    for file in headers:

        filename = "surr_" + file.replace("hpp","cpp")
        if filename not in surrogates:
            print("Surrogate not found for %s" % filename)
            throw_error = True

header_files = get_files("hpp",srcdir)
surrogate_files = get_files("cpp",surrogates_folder)

check_surrogate_exists(header_files,surrogate_files)

if throw_error is True:
    sys.exit(-1)
