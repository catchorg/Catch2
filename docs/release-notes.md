# 1.6.0

### Cmake/ projects:
* Moved CMakeLists.txt to root, made it friendlier for CLion and generating XCode and VS projects, and removed the manually maintained XCode and VS projects.

### Features/ Changes:
* Approx now supports >= and <=
* Can now use \ to escape chars in test names on command line
* Standardize C++11 feature toggles

### Fixes:
* Blue shell colour
* Missing argument to CATCH_CHECK_THROWS
* Don't encode extended ASCII in XML
* use std::shuffle on more compilers (fixes deprecation warning/error)
* Use __COUNTER__ more consistently (where available)

### Other:
* Tweaks and changes to scripts - particularly for Approval test - to make them more portable

# Older versions
Release notes were not maintained prior to v1.6.0, but you should be able to work them out from the Git history

---

[Home](Readme.md)
