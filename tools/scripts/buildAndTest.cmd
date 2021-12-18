rem Start at the root of the Catch project directory, for example:
rem cd Catch2

rem begin-snippet: catch2-build-and-test-win
rem 1. Regenerate the amalgamated distribution
python tools\scripts\generateAmalgamatedFiles.py

rem 2. Configure the full test build
cmake -Bdebug-build -H. -DCMAKE_BUILD_TYPE=Debug -DCATCH_BUILD_EXAMPLES=ON -DCATCH_BUILD_EXTRA_TESTS=ON -DCATCH_DEVELOPMENT_BUILD=ON

rem 3. Run the actual build
cmake --build debug-build

rem 4. Run the tests using CTest
cd debug-build
ctest -j 4 --output-on-failure -C Debug
rem end-snippet
