// RUN: iree-opt -split-input-file --iree-mhlo-to-linalg-on-tensors -canonicalize -cse %s | FileCheck %s

func.func @concatenate(%arg0: tensor<2x2xi32>, %arg1: tensor<2x4xi32>) -> tensor<2x9xi32> {
  %cst = mhlo.constant dense<514> : tensor<2x3xi32>
  %0 = "mhlo.concatenate"(%arg0, %cst, %arg1) {dimension = 1} : (tensor<2x2xi32>, tensor<2x3xi32>, tensor<2x4xi32>) -> tensor<2x9xi32>
  return %0 : tensor<2x9xi32>
}
// CHECK:       func @concatenate
// CHECK-SAME:    %[[ARG0:[a-zA-Z0-9$._-]+]]
// CHECK-SAME:    %[[ARG1:[a-zA-Z0-9$._-]+]]
// CHECK:         %[[CST:.+]] = arith.constant dense<514> : tensor<2x3xi32>
// CHECK:         %[[INIT:.+]] = linalg.init_tensor [2, 9] : tensor<2x9xi32>
// CHECK:         %[[T0:.+]] = tensor.insert_slice %[[ARG0]] into %[[INIT]][0, 0] [2, 2] [1, 1]
// CHECK:         %[[T1:.+]] = tensor.insert_slice %[[CST]] into %[[T0]][0, 2] [2, 3] [1, 1]
// CHECK:         %[[T2:.+]] = tensor.insert_slice %[[ARG1]] into %[[T1]][0, 5] [2, 4] [1, 1]
// CHECK:         return %[[T2]]
