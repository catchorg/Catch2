# How to release

When enough changes have accumulated, it is time to release new version of Catch. This document describes the proces in doing so, that no steps are forgotten. Note that all referenced scripts can be found in the `scripts/` directory.

## Neccessary steps

These steps are neccessary and have to be performed before each new release. They serve to make sure that the new release is correct and linked-to from the standard places.


### Approval testing

Catch's releases are primarily validated against output from previous release, stored in `projects/SelfTest/Baselines`. To validate current sources, build the SelfTest binary and pass it to the `approvalTests.py` script: `approvalTests.py <path/to/SelfTest>`.

There should be no differences, as Approval tests should be updated when changes to Catch are made, but if there are, then they need to be manually reviewed and either approved (using `approve.py`) or Catch requires other fixes.


### Incrementing version number

Catch uses a variant of [semantic versioning](http://semver.org/), with breaking API changes (and thus major version increments) being very rare. Thus, the release will usually increment the patch version, when it only contains couple of bugfixes, or minor version, when it contains new functionality, or larger changes in implementation of current functionality. 

After deciding which part of version number should be incremented, you can use one of the `*Release.py` scripts to perform the required changes to Catch.


### Generate updated single-include header

After updating version number, regenerate single-include header using `generateSingleHeader.py`.


### Release notes

Once a release is ready, release notes need to be written. They should summarize changes done since last release. For rough idea of expected notes see previous releases. Once written, release notes should be placed in `docs/release-notes.md`.


### Commit and push update to GitHub

After version number is incremented, single-include header is regenerated and release notes are updated, changes should be commited and pushed to GitHub. 


### Release on GitHub

After pushing changes to GitHub, GitHub release *needs* to be created. Tag version and release title should be same as the new version, description should contain the release notes for the current release. Single header version of `catch.hpp` *needs* to be attached as a binary, as that is where the official download link links to. Preferably it should use linux line endings.

## Optional steps

The following steps are optional, and do not have to be performed when releasing new version of Catch. However, they are *should* happen, but they can happen the next day without losing anything significant.


### vcpkg update

Catch is maintaining its own port in Microsoft's package manager [vcpkg](https://github.com/Microsoft/vcpkg). This means that when new version of Catch is released, it should be posted there as well. `updateVcpkgPackage.py` can do a lot of neccessary work for you, it creates a branch and commits neccessary changes. You should review these changes, push and open a PR against vcpkg's upstream.

Note that the script assumes you have your fork of vcpkg checked out in a directory next to the directory where you have checked out Catch, like so:
```
GitHub
    Catch
    vcpkg
```


### Wandbox update

Recently we also included a link to wandbox with preloaded Catch on the main page. Strictly speaking it is unneccessary to update this after every release, Catch usually does not change that much between versions, but it should be kept up to date anyway.

