#!/usr/bin/env python3

#              Copyright Catch2 Authors
# Distributed under the Boost Software License, Version 1.0.
#   (See accompanying file LICENSE.txt or copy at
#        https://www.boost.org/LICENSE_1_0.txt)

# SPDX-License-Identifier: BSL-1.0

from typing import List, Tuple
import os
import subprocess
import logging

def configure_and_build(source_path: str, project_path: str, options: List[Tuple[str, str]]):
    configure_cmd = ['cmake', '-B{}'.format(project_path), '-H{}'.format(source_path),
                     '-DCMAKE_BUILD_TYPE=Debug', '-DCATCH_DEVELOPMENT_BUILD=ON']
    for option, value in options:
        configure_cmd.append('-D{}={}'.format(option, value))

    try:
        subprocess.run(configure_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=True)
    except subprocess.CalledProcessError as ex:
        logging.error(f"Could not configure build to '{project_path}' from '{source_path}'")
        logging.error(f"Return code: {ex.returncode}")
        logging.error(f"output: {ex.output}")
        raise

    print(f'Configuring {project_path} finished')

    build_cmd = ['cmake', '--build', '{}'.format(project_path), '--config', 'Debug']
    try:
        subprocess.run(build_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=True)
    except subprocess.CalledProcessError as ex:
        logging.error(f"Could not build project in '{project_path}'")
        logging.error(f"Return code: {ex.returncode}")
        logging.error(f"output: {ex.output}")
        raise

    print(f'Building {project_path} finished')

def run_and_return_output(base_path: str, binary_name: str, other_options: List[str]) -> Tuple[str, str]:
    config_path = "Debug" if os.name == 'nt' else ""
    full_path = os.path.join(base_path, config_path, binary_name)

    cmd = [full_path]
    cmd.extend(other_options)

    try:
        ret = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True, universal_newlines=True)
    except subprocess.CalledProcessError as ex:
        logging.error(f'Could not run "{cmd}"')
        logging.error(f'Args: "{other_options}"')
        logging.error(f'Return code: {ex.returncode}')
        logging.error(f'stdout: {ex.stdout}')
        logging.error(f'stderr: {ex.stdout}')
        raise

    return ret.stdout, ret.stderr

