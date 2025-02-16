// RUN: (iree-compile -iree-hal-target-backends=vmvx -iree-mlir-to-vm-bytecode-module %s | iree-run-module --entry_function=scalar --function_input=42) | FileCheck %s
// RUN: iree-compile -iree-hal-target-backends=vmvx -iree-mlir-to-vm-bytecode-module %s | iree-benchmark-module --driver=vmvx --entry_function=scalar --function_input=42 | FileCheck --check-prefix=BENCHMARK %s
// RUN: (iree-run-mlir --iree-hal-target-backends=vmvx --function-input=42 %s) | FileCheck %s

// BENCHMARK-LABEL: BM_scalar
// CHECK-LABEL: EXEC @scalar
func.func @scalar(%arg0 : i32) -> i32 {
  return %arg0 : i32
}
// CHECK: i32=42
