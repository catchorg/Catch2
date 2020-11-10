#!/usr/bin/env python3
import os
import sys


def get_license():
    with open("src/catch2/catch_all.hpp", "r") as f:
        license = f.readlines()[0:7]

    return license


def check_license(license):
    failed = 0
    base_dir = "src/catch2/"

    # The _ represents the list of directories in base_dir
    for root, _, files in os.walk(base_dir):
        for file in files:
            with open(root + "/" + file, "r") as f:
                file_license = f.readlines()[0:7]

            if file_license != license:
                print("File %s does not have license" % file)
                failed = 1

    return failed


license = get_license()
status = check_license(license)
sys.exit(status)
