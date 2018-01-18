@REM  # In debug build, we want to
@REM  # 1) Prebuild memcheck redirecter
@REM  # 2) Regenerate single header include for examples
@REM  # 3) Enable building examples
if "%CONFIGURATION%"=="Debug" (
  python scripts\generateSingleHeader.py
  cmake -Hmisc -Bbuild-misc -A%PLATFORM%
  cmake --build build-misc
  cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain% -DCATCH_BUILD_EXAMPLES=ON -DMEMORYCHECK_COMMAND=build-misc\Debug\CoverageHelper.exe -DMEMORYCHECK_COMMAND_OPTIONS=--sep-- -DMEMORYCHECK_TYPE=Valgrind
)
if "%CONFIGURATION%"=="Release" (
  cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain%
)
