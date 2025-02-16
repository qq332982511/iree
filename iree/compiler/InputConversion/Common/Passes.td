// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_INPUTCONVERSION_COMMON_PASSES
#define IREE_COMPILER_INPUTCONVERSION_COMMON_PASSES

include "mlir/Pass/PassBase.td"

def IREEImportPublic :
    Pass<"iree-import-public", "ModuleOp"> {
  let summary = "Imports IREE public dialect to internal implementation.";
  let constructor = "mlir::iree_compiler::createIREEImportPublicPass()";
}

def LinalgQuantizedMatmulToMatmulPass
    : Pass<"iree-linalg-quantized-matmul-to-matmul", "func::FuncOp"> {
  let summary = "lower quantized_matmul to matmul";
  let constructor = "mlir::iree_compiler::createLinalgQuantizedMatmulToMatmulPass()";
  // let dependentDialects = ["mlir::linalg::LinalgDialect"];
}

def SanitizeModuleNames :
    Pass<"iree-sanitize-module-names", "ModuleOp"> {
  let summary = "Sanitizes module names for uniformity across target implementations.";
  let constructor = "mlir::iree_compiler::createSanitizeModuleNamesPass()";
}

def TopLevelSCFToCFG :
    Pass<"iree-top-level-scf-to-cfg", "func::FuncOp"> {
  let summary = "Converts non-nested SCF constructs to CFG (not traversing into opaque operations).";
  let constructor = "mlir::iree_compiler::createTopLevelSCFToCFGPass()";
}

#endif // IREE_COMPILER_INPUTCONVERSION_COMMON_PASSES
