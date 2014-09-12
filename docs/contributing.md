# Contributing to Catch

So you want to contribute something to Catch? That's great! Whether it's a bug fix, a new feature, support for additional compilers - or just a fix to the documentation - all contributions are very welcome and very much appreciated. Of course so are bug reports and other comments and questions.

If you are contributing to the code base there are a few simple guidelines to keep in mind. This also includes notes to help you find your way around. As this is liable to drift out of date please raise an issue or, better still, a pull request for this file, if you notice that.

## Branches

Ongoing development is on the "develop" branch, or on feature branches that are branched off of develop. Please target any pull requests at develop, or, for larger chunks of work, a branch off of develop.

## Directory structure

Users of Catch primarily use the single header version. Maintainers should work with the full source (which is still, primarily, in headers). This can be found in the ```include``` folder, but you may prefer to use one of the IDE project files (for MSVC or XCode). These can be found under ```projects/```*IDE Name*```/```*project name*. A number of contributors have proposed make files, and submitted their own versions. At some point these should be made available too. 

In addition to the include files and IDE projects there are a number of tests in cpp files. These can all be found in ```projects/SelfTest```. You'll also see a ```SurrogateCpps``` directory in there. This contains a set of cpp files that each ```#include``` a single header. While these files are not essential to compilation they help to keep the implementation headers self-contained. At time of writing this set is not complete but has reasonable coverage. If you add additional headers please try to remember to add a surrogate cpp for it.

The other directories are ```scripts``` which contains a set of python scripts to help in testing Catch as well as generating the single include, and docs, which contains the documentation as a set of markdown files.

 *This document is a work in progress and will be updated soon with new information.*

---

[Home](../README.md)
