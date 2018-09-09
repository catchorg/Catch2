SETLOCAL EnableDelayedExpansion

cd Build
if "%CONFIGURATION%"=="Debug" (
  if "%coverage%"=="1" (
    ctest -j 2 -C %CONFIGURATION% -D ExperimentalMemCheck || exit /b !ERRORLEVEL!
    python ..\misc\appveyorMergeCoverageScript.py || exit /b !ERRORLEVEL!
    codecov --root .. --no-color --disable gcov -f cobertura.xml -t %CODECOV_TOKEN% || exit /b !ERRORLEVEL!
  ) else (
    ctest -j 2 -C %CONFIGURATION% || exit /b !ERRORLEVEL!
  )
)
if "%CONFIGURATION%"=="Release" (
  ctest -j 2 -C %CONFIGURATION% || exit /b !ERRORLEVEL!
)
