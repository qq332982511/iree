# Copyright 2020 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Default implementations for HAL types that use the host resources.
# These are generally just wrappers around host heap memory and host threads.

load("//iree:build_defs.oss.bzl", "iree_cmake_extra_content")
load("//build_tools/bazel:cc_binary_benchmark.bzl", "cc_binary_benchmark")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "executable_environment",
    srcs = ["executable_environment.c"],
    hdrs = ["executable_environment.h"],
    deps = [
        ":executable_library",
        "//iree/base",
        "//iree/base:tracing",
        "//iree/base/internal:cpu",
        "//iree/hal",
    ],
)

cc_library(
    name = "executable_library",
    hdrs = ["executable_library.h"],
)

cc_binary_benchmark(
    name = "executable_library_benchmark",
    srcs = ["executable_library_benchmark.c"],
    deps = [
        ":executable_environment",
        ":executable_library",
        ":local",
        "//iree/base",
        "//iree/base:tracing",
        "//iree/base/internal:file_io",
        "//iree/base/internal:flags",
        "//iree/hal",
        "//iree/hal/local/loaders:embedded_library_loader",
        "//iree/testing:benchmark",
    ],
)

cc_test(
    name = "executable_library_test",
    srcs = [
        "executable_library_demo.c",
        "executable_library_demo.h",
        "executable_library_test.c",
    ],
    deps = [
        ":executable_environment",
        ":executable_library",
        "//iree/base",
        "//iree/base:core_headers",
    ],
)

cc_library(
    name = "local",
    srcs = [
        "executable_loader.c",
        "inline_command_buffer.c",
        "local_descriptor_set.c",
        "local_descriptor_set_layout.c",
        "local_executable.c",
        "local_executable_cache.c",
        "local_executable_layout.c",
    ],
    hdrs = [
        "executable_loader.h",
        "inline_command_buffer.h",
        "local_descriptor_set.h",
        "local_descriptor_set_layout.h",
        "local_executable.h",
        "local_executable_cache.h",
        "local_executable_layout.h",
    ],
    deps = [
        ":executable_environment",
        ":executable_library",
        "//iree/base",
        "//iree/base:core_headers",
        "//iree/base:tracing",
        "//iree/base/internal",
        "//iree/base/internal:fpu_state",
        "//iree/hal",
    ],
)

cc_library(
    name = "sync_driver",
    srcs = [
        "sync_device.c",
        "sync_driver.c",
        "sync_event.c",
        "sync_semaphore.c",
    ],
    hdrs = [
        "sync_device.h",
        "sync_driver.h",
        "sync_event.h",
        "sync_semaphore.h",
    ],
    deps = [
        ":local",
        "//iree/base",
        "//iree/base:core_headers",
        "//iree/base:tracing",
        "//iree/base/internal",
        "//iree/base/internal:arena",
        "//iree/base/internal:synchronization",
        "//iree/hal",
        "//iree/hal/utils:buffer_transfer",
    ],
)

#===------------------------------------------------------------------------===#
# Thread dependent packages
#===------------------------------------------------------------------------===#

iree_cmake_extra_content(
    content = """
# task_driver is used by asynchronuous drivers.
# TODO(scotttodd): refactor this - code depending on threading should be
#   possible to declare in the build system but conditionally link in
if(NOT EMSCRIPTEN AND NOT (${IREE_HAL_DRIVER_DYLIB} OR ${IREE_HAL_DRIVER_VMVX}))
  return()
endif()
""",
    inline = True,
)

cc_library(
    name = "task_driver",
    srcs = [
        "task_command_buffer.c",
        "task_device.c",
        "task_driver.c",
        "task_event.c",
        "task_queue.c",
        "task_queue_state.c",
        "task_semaphore.c",
    ],
    hdrs = [
        "task_command_buffer.h",
        "task_device.h",
        "task_driver.h",
        "task_event.h",
        "task_queue.h",
        "task_queue_state.h",
        "task_semaphore.h",
    ],
    deps = [
        ":executable_environment",
        ":executable_library",
        ":local",
        "//iree/base",
        "//iree/base:core_headers",
        "//iree/base:tracing",
        "//iree/base/internal",
        "//iree/base/internal:arena",
        "//iree/base/internal:event_pool",
        "//iree/base/internal:synchronization",
        "//iree/base/internal:wait_handle",
        "//iree/hal",
        "//iree/hal/utils:buffer_transfer",
        "//iree/hal/utils:resource_set",
        "//iree/task",
    ],
)
