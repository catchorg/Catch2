#!/usr/bin/env sh

# Start at the root of the Catch project directory, for example:
# cd Catch2

# begin-snippet: catch2-build-and-test
# 1. Regenerate the amalgamated distribution
./tools/scripts/generateAmalgamatedFiles.py

# 2. Configure the full test build
cmake -Bdebug-build -H. -DCMAKE_BUILD_TYPE=Debug -DCATCH_DEVELOPMENT_BUILD=ON -DCATCH_BUILD_EXAMPLES=ON -DCATCH_BUILD_EXTRA_TESTS=ON -DCATCH_ENABLE_CONFIGURE_TESTS=ON

# 3. Run the actual build
cmake --build debug-build

# 4. Run the tests using CTest
cd debug-build
ctest -j 4 --output-on-failure -C Debug
# end-snippet
