load("@cpptest//:cpptest.bzl", "cpptest_create")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "typecheck",
    srcs = glob([
        "src/**/*.cpp",
    ], exclude = [
        "src/**/*.test.cpp",
    ]),
    hdrs = glob(["include/**/*.hpp", "src/**/*.hpp"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    copts = ["-std=c++20"],
    deps = [
        "@magic_enum",
        "@cppnotstdlib",
        "@constraint"
    ],
)

cpptest_create(
    name = "typecheck",
    srcs = glob(["src/**/*.test.cpp"]),
)

# Remote-execution platform for CI (--config=ci --host_platform=...).
platform(
    name = "docker_image_platform",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "@bazel_tools//tools/cpp:clang",
    ],
    exec_properties = {
        "OSFamily": "Linux",
        "container-image": "docker://ghcr.io/mratedca/bazel_cpp_worker:latest",
    },
)
