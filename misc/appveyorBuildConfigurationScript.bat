
@REM  # Possibilities:
@REM  # Debug build + coverage
@REM  # Debug build + examples
@REM  # Debug build +   ---
@REM  # Release build
if "%CONFIGURATION%"=="Debug" (
  if "%coverage%"=="1" (
    @REM # coverage needs to build the special helper as well as the main
    cmake -Hmisc -Bbuild-misc -A%PLATFORM%
    cmake --build build-misc
    cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain% -DMEMORYCHECK_COMMAND=build-misc\Debug\CoverageHelper.exe -DMEMORYCHECK_COMMAND_OPTIONS=--sep-- -DMEMORYCHECK_TYPE=Valgrind
  ) else (
    @REM # We know that coverage is 0
    if "%examples%"=="1" (
      @REM # Examples live off the single header, so it needs to be regenerated
      python scripts\generateSingleHeader.py
      cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain% -DCATCH_BUILD_EXAMPLES=ON
    ) else (
      @REM # This is just a plain debug build
      cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain%
    )
  )
)
if "%CONFIGURATION%"=="Release" (
  cmake -H. -BBuild -A%PLATFORM% -DUSE_WMAIN=%wmain%
)
