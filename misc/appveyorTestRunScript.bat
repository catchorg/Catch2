cd Build
if "%CONFIGURATION%"=="Debug" (
  ctest -j 2 -C %CONFIGURATION% -D ExperimentalMemCheck
  python ..\misc\appveyorMergeCoverageScript.py
  codecov --root .. --no-color --disable gcov -f cobertura.xml -t %CODECOV_TOKEN%
)
if "%CONFIGURATION%"=="Release" (
  ctest -j 2 -C %CONFIGURATION%
)
