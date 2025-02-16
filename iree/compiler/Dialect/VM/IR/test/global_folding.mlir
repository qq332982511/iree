// Tests folding and canonicalization of global ops.

// RUN: iree-opt -split-input-file -pass-pipeline="vm.module(canonicalize)" %s | FileCheck %s

// CHECK-LABEL: @global_i32_folds
vm.module @global_i32_folds {
  // CHECK: vm.global.i32 public mutable @g0 = 123 : i32
  vm.global.i32 mutable @g0 : i32
  vm.initializer {
    %c123 = vm.const.i32 123
    vm.global.store.i32 %c123, @g0 : i32
    vm.return
  }

  // CHECK: vm.global.i32 public mutable @g1 : i32
  vm.global.i32 mutable @g1 = 0 : i32
  // CHECK: vm.global.i32 public @g2 : i32
  vm.global.i32 @g2 = 0 : i32

  // CHECK: vm.global.i32 public mutable @g3 : i32
  vm.global.i32 mutable @g3 : i32
  vm.initializer {
    %c0 = vm.const.i32 0
    vm.global.store.i32 %c0, @g3 : i32
    vm.return
  }
}

// -----

// CHECK-LABEL: @global_ref_folds_null
vm.module @global_ref_folds_null {
  // CHECK: vm.global.ref public mutable @g0 : !vm.ref<?>
  vm.global.ref mutable @g0 : !vm.ref<?>
  vm.initializer {
    %null = vm.const.ref.zero : !vm.ref<?>
    vm.global.store.ref %null, @g0 : !vm.ref<?>
    vm.return
  }
}

// -----

// CHECK-LABEL: @global_load_i32_folds
vm.module @global_load_i32_folds {
  vm.global.i32 @g0 = 123 : i32
  // CHECK-LABEL: @inline_const_value
  vm.func @inline_const_value() -> i32 {
    // CHECK-NEXT: %c123 = vm.const.i32 123
    // CHECK-NEXT: vm.return %c123 : i32
    %g0 = vm.global.load.i32 @g0 : i32
    vm.return %g0 : i32
  }

  vm.global.i32 mutable @g1 = 123 : i32
  // CHECK-LABEL: @ignore_nonconst_value
  vm.func @ignore_nonconst_value() -> i32 {
    // NOTE: ensure we don't inline non-constant values.
    // CHECK-NEXT: %g1 = vm.global.load.i32 @g1 : i32
    // CHECK-NEXT: vm.return %g1 : i32
    %g1 = vm.global.load.i32 @g1 : i32
    vm.return %g1 : i32
  }
}

// -----

// CHECK-LABEL: @global_load_ref_folds
vm.module @global_load_ref_folds {
  vm.global.ref @g0 : !vm.ref<?>
  // CHECK-LABEL: @inline_const_null
  vm.func @inline_const_null() -> !vm.ref<?> {
    // CHECK-NEXT: %null = vm.const.ref.zero : !vm.ref<?>
    // CHECK-NEXT: vm.return %null : !vm.ref<?>
    %g0 = vm.global.load.ref @g0 : !vm.ref<?>
    vm.return %g0 : !vm.ref<?>
  }

  vm.global.ref mutable @g1 : !vm.ref<?>
  // CHECK-LABEL: @ignore_nonconst_value
  vm.func @ignore_nonconst_value() -> !vm.ref<?> {
    // NOTE: ensure we don't inline non-constant values.
    // CHECK-NEXT: %g1 = vm.global.load.ref @g1 : !vm.ref<?>
    // CHECK-NEXT: vm.return %g1 : !vm.ref<?>
    %g1 = vm.global.load.ref @g1 : !vm.ref<?>
    vm.return %g1 : !vm.ref<?>
  }
}

// -----

// CHECK-LABEL: @global_indirect_folds
vm.module @global_indirect_folds {
  vm.global.i32 mutable @g0 : i32

  // CHECK-LABEL: @fold_load_i32
  vm.func @fold_load_i32() -> i32 {
    %0 = vm.global.address @g0 : !util.ptr<i32>
    // CHECK-NEXT: %[[VALUE:.+]] = vm.global.load.i32 @g0 : i32
    %1 = vm.global.load.indirect.i32 %0 : !util.ptr<i32> -> i32
    // CHECK-NEXT: vm.return %[[VALUE]]
    vm.return %1 : i32
  }

  // CHECK-LABEL: @fold_store_i32
  vm.func @fold_store_i32(%arg0 : i32) {
    %0 = vm.global.address @g0 : !util.ptr<i32>
    // CHECK-NEXT: vm.global.store.i32 %arg0, @g0 : i32
    vm.global.store.indirect.i32 %arg0, %0 : i32 -> !util.ptr<i32>
    vm.return
  }
}
