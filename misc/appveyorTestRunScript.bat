cd Build
if "%CONFIGURATION%"=="Debug" (
  if "%coverage%"=="1" (
    ctest -j 2 -C %CONFIGURATION% -D ExperimentalMemCheck
    python ..\misc\appveyorMergeCoverageScript.py
    codecov --root .. --no-color --disable gcov -f cobertura.xml -t %CODECOV_TOKEN%
  ) else (
    ctest -j 2 -C %CONFIGURATION%
  )
)
if "%CONFIGURATION%"=="Release" (
  ctest -j 2 -C %CONFIGURATION%
)
