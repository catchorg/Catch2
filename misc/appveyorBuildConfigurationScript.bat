@REM  # In debug build, we want to
@REM  # 1) Prebuild memcheck redirecter
@REM  # 2) Regenerate single header include for examples
@REM  # 3) Enable building examples
if "%CONFIGURATION%"=="Debug" (
  echo "buildConfiguration.bat thinks this is a Debug build"
  python scripts\generateSingleHeader.py
  cmake -Hmisc -Bbuild-misc -A%PLATFORM%
  cmake --build build-misc
  cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain% -DBUILD_EXAMPLES=ON -DMEMORYCHECK_COMMAND=build-misc\Debug\CoverageHelper.exe -DMEMORYCHECK_COMMAND_OPTIONS=--sep-- -DMEMORYCHECK_TYPE=Valgrind
)
if "%CONFIGURATION%"=="Release" (
  echo "buildConfiguration.bat thinks this is a Release build"
  cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain%
)
