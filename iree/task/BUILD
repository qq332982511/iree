# Copyright 2020 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("//iree:build_defs.oss.bzl", "iree_cmake_extra_content")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

iree_cmake_extra_content(
    content = """
# Task-based executor requires threading support.
if(NOT ${IREE_ENABLE_THREADING})
  return()
endif()
""",
    inline = True,
)

cc_library(
    name = "api",
    srcs = ["api.c"],
    hdrs = ["api.h"],
    deps = [
        ":task",
        "//iree/base:tracing",
        "//iree/base/internal:flags",
    ],
)

cc_library(
    name = "task",
    srcs = [
        "executor.c",
        "executor_impl.h",
        "list.c",
        "poller.c",
        "pool.c",
        "post_batch.c",
        "post_batch.h",
        "queue.c",
        "scope.c",
        "submission.c",
        "task.c",
        "task_impl.h",
        "topology.c",
        "topology_cpuinfo.c",
        "worker.c",
        "worker.h",
    ],
    hdrs = [
        "affinity_set.h",
        "executor.h",
        "list.h",
        "poller.h",
        "pool.h",
        "queue.h",
        "scope.h",
        "submission.h",
        "task.h",
        "topology.h",
        "topology_cpuinfo.h",
        "tuning.h",
    ],
    deps = [
        "//iree/base",
        "//iree/base:core_headers",
        "//iree/base:tracing",
        "//iree/base/internal",
        "//iree/base/internal:atomic_slist",
        "//iree/base/internal:cpu",
        "//iree/base/internal:event_pool",
        "//iree/base/internal:fpu_state",
        "//iree/base/internal:prng",
        "//iree/base/internal:synchronization",
        "//iree/base/internal:threading",
        "//iree/base/internal:wait_handle",
        "@cpuinfo",
    ],
)

cc_test(
    name = "executor_demo",
    srcs = ["executor_demo.cc"],
    deps = [
        ":task",
        "//iree/base",
        "//iree/base:tracing",
        "//iree/base/internal:prng",
        "//iree/task/testing:test_util",
    ],
)

cc_test(
    name = "executor_test",
    srcs = ["executor_test.cc"],
    deps = [
        ":task",
        "//iree/base",
        "//iree/task/testing:test_util",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "list_test",
    srcs = ["list_test.cc"],
    deps = [
        ":task",
        "//iree/base",
        "//iree/task/testing:test_util",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "pool_test",
    srcs = ["pool_test.cc"],
    deps = [
        ":task",
        "//iree/base",
        "//iree/task/testing:test_util",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "queue_test",
    srcs = ["queue_test.cc"],
    deps = [
        ":task",
        "//iree/base",
        "//iree/task/testing:test_util",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "scope_test",
    srcs = [
        "scope_test.cc",
        "task_impl.h",
    ],
    deps = [
        ":task",
        "//iree/base",
        "//iree/task/testing:test_util",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "task_tests",
    srcs = [
        "task_test_barrier.cc",
        "task_test_call.cc",
        "task_test_dispatch.cc",
        "task_test_fence.cc",
        "task_test_nop.cc",
        "task_test_wait.cc",
    ],
    deps = [
        ":task",
        "//iree/base",
        "//iree/task/testing:task_test",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "topology_test",
    srcs = ["topology_test.cc"],
    deps = [
        ":task",
        "//iree/base",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)

cc_test(
    name = "topology_cpuinfo_test",
    srcs = ["topology_cpuinfo_test.cc"],
    tags = [
        "noasan",  # TODO(benvanik): Does not work on machines with large numbers of cores.
    ],
    deps = [
        ":task",
        "//iree/base",
        "//iree/testing:gtest",
        "//iree/testing:gtest_main",
    ],
)
