#!/bin/sh
#
# Builds the fuzzers
#
# By Paul Dreik 20200923
set -exu

CATCHROOT=$(readlink -f $(dirname $0)/..)


BUILDDIR=$CATCHROOT/build-fuzzers
mkdir -p $BUILDDIR
cd $BUILDDIR

cmake $CATCHROOT \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_CXX_FLAGS="-fsanitize=fuzzer-no-link,address,undefined -O3 -g" \
  -DCATCH_BUILD_EXAMPLES=Off \
  -DCATCH_BUILD_EXTRA_TESTS=Off \
  -DCATCH_BUILD_TESTING=Off \
  -DBUILD_TESTING=Off \
  -DCATCH_BUILD_FUZZERS=On

cmake --build . -j 4

