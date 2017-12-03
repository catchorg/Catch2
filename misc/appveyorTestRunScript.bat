cd Build
if "%CONFIGURATION%"=="Debug" (
  echo "appveyorTestScript.bat thinks this is a Debug build"
  ctest -j 2 -C %CONFIGURATION% -D ExperimentalMemCheck
  python ..\misc\appveyorMergeCoverageScript.py
  codecov --root .. --no-color --disable gcov -f cobertura.xml -t %CODECOV_TOKEN%
)
if "%CONFIGURATION%"=="Release" (
  echo "appveyorTestScript.bat thinks this is a Release build"
  ctest -j 2 -C %CONFIGURATION%
)
