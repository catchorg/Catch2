load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
http_archive(
    name = "bazel_skylib",
    urls = [
        "https://github.com/Vertexwahn/bazel-skylib/archive/b0cd4bbd4bf4af76c380e1f8fafdbe3964161aff.tar.gz",
    ],
    strip_prefix = "bazel-skylib-b0cd4bbd4bf4af76c380e1f8fafdbe3964161aff",
    sha256 = "e57f3ff541c65678f3c2b344c73945531838e86ea0be71c63eea862ab43e792b",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()
