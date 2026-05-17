<a id="top"></a>
# How to release

When enough changes have accumulated, it is time to release new version of Catch. This document describes the process in doing so, that no steps are forgotten. Note that all referenced scripts can be found in the `tools/scripts/` directory.

## Necessary steps

These steps are necessary and have to be performed before each new release. They serve to make sure that the new release is correct and linked-to from the standard places.


### Testing

All of the tests are run in GitHub Actions and AppVeyor. As long as the
last commit on the release branch tested green, the release can proceed.


### Incrementing version number

Catch uses a variant of [semantic versioning](http://semver.org/), with breaking API changes (and thus major version increments) being very rare. Thus, the release will usually increment the patch version, when it only contains couple of bugfixes, or minor version, when it contains new functionality, or larger changes in implementation of current functionality.

After deciding which part of version number should be incremented, you can use one of the `*Release.py` scripts to perform the required changes to Catch.

This will take care of generating the single include header and updating
version numbers everywhere.


### "Try online" link (Compiler Explorer)

The README **Try online** badge should point to a [Compiler Explorer](https://godbolt.org/)
session that uses Catch2's prebuilt library (not uploaded sources). CE refreshes
its library builds on a regular schedule, so a link created for release `X.Y.Z`
may only show that exact version after CE has picked up the package.

When cutting a release:

1. Open Compiler Explorer and configure a C++23 (or your target) environment with
   the **Catch2** library enabled (the packaged `catchorg/catch2` entry).
2. Add a minimal example (e.g. the factorial test from the README) and confirm it
   compiles and runs.
3. Use **Share** → **Permanent link** and copy the short URL (`https://godbolt.org/z/...`).
4. Update the badge URL in the root [`README.md`](../README.md) (`[![Try online](...)]`).
5. Optionally add a similar link to the new GitHub release description.

Full automation is tracked in [#2330](https://github.com/catchorg/Catch2/issues/2330);
until then this step is manual.


### Release notes

Once a release is ready, release notes need to be written. They should summarize changes done since last release. For rough idea of expected notes see previous releases. Once written, release notes should be added to `docs/release-notes.md`.


### Commit and push update to GitHub

After version number is incremented, single-include header is regenerated and release notes are updated, changes should be committed and pushed to GitHub.


### Release on GitHub

After pushing changes to GitHub, GitHub release *needs* to be created.
Tag version and release title should be same as the new version,
description should contain the release notes for the current release.
We also attach the two amalgamated files as "binaries".

Since 2.5.0, the release tag and the "binaries" (amalgamated files) should
be PGP signed.

#### Signing a tag

To create a signed tag, use `git tag -s <VERSION>`, where `<VERSION>`
is the version being released, e.g. `git tag -s v2.6.0`.

Use the version name as the short message and the release notes as
the body (long) message.

#### Signing the amalgamated files

This will create ASCII-armored signatures for the two amalgamated files
that are uploaded to the GitHub release:

```
gpg --armor --output extras/catch_amalgamated.hpp.asc --detach-sig extras/catch_amalgamated.hpp
gpg --armor --output extras/catch_amalgamated.cpp.asc --detach-sig extras/catch_amalgamated.cpp
```

_GPG does not support signing multiple files in single invocation._
