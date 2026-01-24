load("@cpptest//:cpptest.bzl", "cpptest_create")

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
