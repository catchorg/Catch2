#!/usr/bin/env python

import io, os, re, sys, subprocess
import hashlib

from scriptCommon import catchPath
from releaseCommon import Version

print(catchPath)

default_path = '../vcpkg/ports/catch/'

def adjusted_path(path):
    return os.path.join(catchPath, path)

def get_hash(path):
    BUFF_SIZE = 65536
    sha512 = hashlib.sha512()
    # The newlines should be normalized into \n, which is what we want
    # If reused use 'rb' with a file written with io.open(newline='\n')
    with open(path, 'r') as f:
        while True:
            data = f.read(BUFF_SIZE)
            if not data:
                break
            if sys.version_info[0] < 3:
                sha512.update(data)
            else:
                sha512.update(data.encode('utf-8'))
    return sha512.hexdigest()

def update_control(path):
    v = Version()
    ver_string = v.getVersionString()

    # Update control
    lines = []
    control_path = os.path.join(path, 'CONTROL')
    with open(control_path, 'r') as f:
        for line in f:
            lines.append(line)
    with open(control_path, 'w') as f:
        for line in lines:
            if 'Version: ' in line:
                line = 'Version: {}\n'.format(v.getVersionString())
            f.write(line)

def update_portfile(path, header_hash, licence_hash):
    print('Updating portfile')
    v = Version()
    ver_string = v.getVersionString()

    # Update portfile
    lines = []
    portfile_path = os.path.join(path, 'portfile.cmake')
    with open(portfile_path, 'r') as f:
        for line in f:
            lines.append(line)
    with open(portfile_path, 'w') as f:
        # There are three things we need to change/update
        # 1) CATCH_VERSION cmake variable
        # 2) Hash of header
        # 3) Hash of licence
        # We could assume licence never changes, but where is the fun in that?
        for line in lines:
            # Update the version
            if 'set(CATCH_VERSION' in line:
                line = 'set(CATCH_VERSION v{})'.format(v.getVersionString())

            # Determine which file we are updating
            if 'vcpkg_download_distfile' in line:
                kind = line.split('(')[-1].strip()

            # Update the hashes
            if 'SHA512' in line and kind == 'HEADER':
                line = '    SHA512 {}\n'.format(header_hash)
            if 'SHA512' in line and kind == 'LICENSE':
                line = '    SHA512 {}\n'.format(licence_hash)
            f.write(line)


def git_push(path_to_repo):
    v = Version()
    ver_string = v.getVersionString()

    os.chdir(path_to_repo)

    # Work with git
    # Make sure we branch off master
    subprocess.call('git checkout master', shell=True)
    
    # Update repo to current master, so we don't work off old version of the portsfile 
    subprocess.call('git pull Microsoft master', shell=True)
    subprocess.call('git push', shell=True)

    # Create a new branch for the update
    subprocess.call('git checkout -b catch-{}'.format(ver_string), shell=True)
    # Add changed files (should be only our files)
    subprocess.call('git add -u .', shell=True)
    # Create a commit with these changes
    subprocess.call('git commit -m "Update Catch to {}"'.format(ver_string), shell=True)
    # Don't push, so author can review
    print('Changes were commited to the vcpkg fork. Please check, push and open PR.')

header_hash = get_hash(adjusted_path('single_include/catch.hpp'))
licence_hash = get_hash(adjusted_path('LICENSE.txt'))
update_control(adjusted_path(default_path))
update_portfile(adjusted_path(default_path), header_hash, licence_hash)

git_push(adjusted_path('../vcpkg'))
