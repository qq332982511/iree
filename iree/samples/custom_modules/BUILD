# Copyright 2019 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("//build_tools/bazel:iree_bytecode_module.bzl", "iree_bytecode_module")
load("//iree:build_defs.oss.bzl", "iree_cmake_extra_content")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "module",
    srcs = ["module.cc"],
    hdrs = ["module.h"],
    deps = [
        "//iree/base",
        "//iree/base:cc",
        "//iree/hal",
        "//iree/modules/hal",
        "//iree/vm",
        "//iree/vm:cc",
    ],
)

iree_cmake_extra_content(
    content = """
if(NOT "${IREE_TARGET_BACKEND_VMVX}" OR
   NOT "${IREE_HAL_DRIVER_VMVX}")
  return()
endif()
""",
)

iree_bytecode_module(
    name = "custom_modules_test_module",
    src = "custom_modules_test.mlir",
    c_identifier = "iree_samples_custom_modules_test_module",
    flags = [
        "-iree-mlir-to-vm-bytecode-module",
        "-iree-hal-target-backends=vmvx",
    ],
    translate_tool = "//iree/samples/custom_modules/dialect:custom-translate",
)

cc_test(
    name = "custom_modules_test",
    srcs = ["custom_modules_test.cc"],
    deps = [
        ":custom_modules_test_module_c",
        ":module",
        "//iree/base",
        "//iree/base:logging",
        "//iree/hal",
        "//iree/hal/vmvx/registration",
        "//iree/modules/hal",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
        "//iree/vm",
        "//iree/vm:bytecode_module",
        "//iree/vm:cc",
    ],
)
