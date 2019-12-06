#!/usr/bin/env python

from __future__ import print_function

import time, subprocess, sys, os, shutil, glob, random
import argparse

def median(lst):
    lst = sorted(lst)
    mid, odd = divmod(len(lst), 2)
    if odd:
        return lst[mid]
    else:
        return (lst[mid - 1] + lst[mid]) / 2.0

def mean(lst):
    return float(sum(lst)) / max(len(lst), 1)

compiler_path = ''
flags = []

main_file = r'''
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
'''
main_name = 'catch-main.cpp'

dir_name = 'benchmark-dir'

files = 20
test_cases_in_file = 20
sections_in_file = 4
assertions_per_section = 5

checks = [
    'a != b', 'a != c', 'a != d', 'a != e', 'b != c', 'b != d', 'b != e', 'c != d', 'c != e', 'd != e', 'a + a == a',
    'a + b == b', 'a + c == c', 'a + d == d', 'a + e == e', 'b + a == b', 'b + b == c', 'b + c == d',
    'b + d == e', 'c + a == c', 'c + b == d', 'c + c == e', 'd + a == d', 'd + b == e', 'e + a == e',
    'a + a + a == a', 'b + c == a + d', 'c + a + a == a + b + b + a',
    'a < b', 'b < c', 'c < d', 'd < e', 'a >= a', 'd >= b',
]

def create_temp_dir():
    if os.path.exists(dir_name):
        shutil.rmtree(dir_name)
    os.mkdir(dir_name)

def copy_catch(path_to_catch):
    shutil.copy(path_to_catch, dir_name)

def create_catch_main():
    with open(main_name, 'w') as f:
        f.write(main_file)

def compile_main():
    start_t = time.time()
    subprocess.check_call([compiler_path, main_name, '-c'] + flags)
    end_t = time.time()
    return end_t - start_t

def compile_files():
    cpp_files = glob.glob('tests*.cpp')
    start_t = time.time()
    subprocess.check_call([compiler_path, '-c'] + flags + cpp_files)
    end_t = time.time()
    return end_t - start_t

def link_files():
    obj_files = glob.glob('*.o')
    start_t = time.time()
    subprocess.check_call([compiler_path] + flags + obj_files)
    end_t = time.time()
    return end_t - start_t

def benchmark(func):
    results = [func() for i in range(10)]
    return mean(results), median(results)

def char_range(start, end):
    for c in range(ord(start), ord(end)):
        yield chr(c)

def generate_sections(fd):
    for i in range(sections_in_file):
        fd.write('    SECTION("Section {}") {{\n'.format(i))
        fd.write('\n'.join('        CHECK({});'.format(check) for check in random.sample(checks, assertions_per_section)))
        fd.write('    }\n')


def generate_file(file_no):
    with open('tests{}.cpp'.format(file_no), 'w') as f:
        f.write('#include "catch.hpp"\n\n')
        for i in range(test_cases_in_file):
            f.write('TEST_CASE("File {} test {}", "[.compile]"){{\n'.format(file_no, i))
            for i, c in enumerate(char_range('a', 'f')):
                f.write('    int {} = {};\n'.format(c, i))
            generate_sections(f)
            f.write('}\n\n')


def generate_files():
    create_catch_main()
    for i in range(files):
        generate_file(i)


options = ['all', 'main', 'files', 'link']

parser = argparse.ArgumentParser(description='Benchmarks Catch\'s compile times against some synthetic tests')
# Add first arg -- benchmark type
parser.add_argument('benchmark_kind', nargs='?', default='all', choices=options, help='What kind of benchmark to run, default: all')

# Args to allow changing header/compiler
parser.add_argument('-I', '--catch-header', default='catch.hpp', help = 'Path to catch.hpp, default: catch.hpp')
parser.add_argument('-c', '--compiler', default='g++', help = 'Compiler to use, default: g++')

parser.add_argument('-f', '--flags', help = 'Flags to be passed to the compiler. Pass as "," separated list')

# Allow creating files only, without running the whole thing
parser.add_argument('-g', '--generate-files', action='store_true', help='Generate test files and quit')

args = parser.parse_args()

compiler_path = args.compiler
catch_path = args.catch_header

if args.generate_files:
    create_temp_dir()
    copy_catch(catch_path)
    os.chdir(dir_name)
    # now create the fake test files
    generate_files()
    # Early exit
    print('Finished generating files')
    exit(1)

os.chdir(dir_name)

if args.flags:
    flags = args.flags.split(',')

print('Time needed for ...')
if args.benchmark_kind in ('all', 'main'):
    print('        ... compiling main, mean: {:.2f}, median: {:.2f} s'.format(*benchmark(compile_main)))
if args.benchmark_kind in ('all', 'files'):
    print('        ... compiling test files, mean: {:.2f}, median: {:.2f} s'.format(*benchmark(compile_files)))
if args.benchmark_kind in ('all', 'link'):
    print('        ... linking everything, mean: {:.2f}, median: {:.2f} s'.format(*benchmark(link_files)))
