// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_CODEGEN_COMMON_DESTRUCTIVEUPDATEUTILS_H_
#define IREE_COMPILER_CODEGEN_COMMON_DESTRUCTIVEUPDATEUTILS_H_

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"

namespace mlir {
namespace iree_compiler {

// Traverse `funcOp` and rewrite specific tensor.{extract|insert}_slice ops that
// match a "destructive tensor update" pattern, by an inplace update via
// flow.dispatch.tensor.{load|store} ops.
//
// This serves as a step in jumping the abstraction gap between transformed
// "linalg on tensors" IR (in which the whole tensor is updated) and dispatch
// regions (in which each workgroup updates a non-overlapping portion of the
// output tensors).
// This is possible because we control the production of such patterns in IREE
// and can take the necessary shortcuts wrt inplace semantics.
// In the future it is reasonable to expect special IR constructs to capture
// some of the destructive update patterns.
//
// Assumptions/Invariants on "Control the Production of Such Patterns"
// ===================================================================
// 1. Only output tensors may be written by a destructive update pattern.
// 2. extract_slice/insert_slice ops are the only ops that can extract/insert
//    from/into tensors.
// 3. All extract_slice/insert_slice must have been introduced by Linalg
//    tiling on tensors.
// 4. Such tilings that result in yielded tensors across loops may only tile
//    parallel Linalg iterators atm.
// 5. (Future) Allow non-parallel Linalg iterators tiling and ensure first-read
//    or writeOnly by construction.
//
// The following destructive update patterns are rewritten.
//
// Coming from an `Flow::DispatchTensorLoadOp`
// ==========================================
// ```
//   %0 = flow.dispatch.tensor.load %a : !flow.dispatch.tensor<readonly:...> ->
//   tensor<...>
//   ...
//   %1 = destructive_update(%0)
//   ...
//   use_of(%1) // e.g. flow.dispatch.tensor.store %1, %b :
//              //        tensor<...> -> !flow.dispatch.tensor<writeonly:...>
// ```
// is rewritten into:
// ```
//   %0 = flow.dispatch.tensor.load %a : !flow.dispatch.tensor<readonly:...> ->
//   tensor<...>
//   ...
//   inplace_update(%0, %out) //e.g. flow.dispatch.tensor.store %subtensor, %b,
//                            //     offsets = ..., sizes = ..., strides = ... :
//                            //       tensor<...> ->
//                            !flow.dispatch.tensor<writeonly:...>
//   %2 = flow.dispatch.tensor.load %b
//   ...
//   use_of(%2) // e.g. flow.dispatch.tensor.store %2, %b :
//              //        tensor<...> -> !flow.dispatch.tensor<writeonly:...>
// ```
//
// This is a typical pattern that appears after tiling Linalg ops on tensors
// with operands that come from flow.dispatch.tensor.
//
// Other rewrites:
// ===============
// Furthermore, when no interleaved aliasing write to %b occurs, the following:
// ```
//   %2 = flow.dispatch.tensor.load %b
//   ...
//   flow.dispatch.tensor.store %2, %b :
//     tensor<...> -> !flow.dispatch.tensor<writeonly:...>
// ```
// is elided.
LogicalResult rewriteLinalgDestructiveUpdates(func::FuncOp parentOp);

}  // namespace iree_compiler
}  // namespace mlir

#endif  // IREE_COMPILER_CODEGEN_COMMON_DESTRUCTIVEUPDATEUTILS_H_
