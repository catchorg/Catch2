#!/usr/bin/env python2

import glob
import subprocess

if __name__ == '__main__':
    cov_files = list(glob.glob('cov-report*.bin'))
    base_cmd = ['OpenCppCoverage', '--quiet', '--export_type=cobertura:cobertura.xml'] + ['--input_coverage={}'.format(f) for f in cov_files]
    subprocess.call(base_cmd)
