// RUN: iree-run-mlir --iree-hal-target-backends=dylib-llvm-aot --iree-llvm-link-embedded=true -mlir-disable-threading --iree-llvm-target-cpu-features='host' --iree-codegen-llvm-generic-ops-workgroup-size=2048 %s

//===----------------------------------------------------------------------===//
// Transpose ops.
// Naming convention: '_'.join(
//   [transpose,
//    {transpose-pattern])
//
//===----------------------------------------------------------------------===//

util.global private @"__transpose_10_input" {noinline} = dense<1.0> : tensor<512x1024xf32>

func @transpose_10() -> tensor<1024x512xf32> {
  %cst = arith.constant 0.000000e+00 : f32
  %c0 = arith.constant 0 : index
  %input_ptr = util.global.address @"__transpose_10_input" : !util.ptr<tensor<512x1024xf32>>
  %input = util.global.load.indirect %input_ptr : !util.ptr<tensor<512x1024xf32>> -> tensor<512x1024xf32>
  %output = linalg.init_tensor [1024, 512] : tensor<1024x512xf32>
  %6 = linalg.generic {
    indexing_maps = [ affine_map<(d0, d1) -> (d1, d0)>, affine_map<(d0, d1) -> (d0, d1)>],
    iterator_types = ["parallel", "parallel"]}
    ins(%input : tensor<512x1024xf32>) outs(%output : tensor<1024x512xf32>) {
    ^bb0(%arg1: f32, %arg2: f32):
      linalg.yield %arg1 : f32
    } -> tensor<1024x512xf32>
  return %6: tensor<1024x512xf32>
}

util.global private @"__transpose_021_input" {noinline} = dense<1.0> : tensor<64x96x128xf32>

func @transpose_021() -> tensor<64x128x96xf32> {
  %cst = arith.constant 0.000000e+00 : f32
  %c0 = arith.constant 0 : index
  %input_ptr = util.global.address @"__transpose_021_input" : !util.ptr<tensor<64x96x128xf32>>
  %input = util.global.load.indirect %input_ptr : !util.ptr<tensor<64x96x128xf32>> -> tensor<64x96x128xf32>
  %output = linalg.init_tensor [64, 128, 96] : tensor<64x128x96xf32>
  %6 = linalg.generic {
    indexing_maps = [ affine_map<(d0, d1, d2) -> (d0, d2, d1)>, affine_map<(d0, d1, d2) -> (d0, d1, d2)>],
    iterator_types = ["parallel", "parallel", "parallel"]}
    ins(%input : tensor<64x96x128xf32>) outs(%output : tensor<64x128x96xf32>) {
    ^bb0(%arg1: f32, %arg2: f32):
      linalg.yield %arg1 : f32
    } -> tensor<64x128x96xf32>
  return %6: tensor<64x128x96xf32>
}

util.global private @"__transpose_201_input" {noinline} = dense<1.0> : tensor<64x96x128xf32>

func @transpose_201() -> tensor<128x64x96xf32> {
  %cst = arith.constant 0.000000e+00 : f32
  %c0 = arith.constant 0 : index
  %input_ptr = util.global.address @"__transpose_201_input" : !util.ptr<tensor<64x96x128xf32>>
  %input = util.global.load.indirect %input_ptr : !util.ptr<tensor<64x96x128xf32>> -> tensor<64x96x128xf32>
  %output = linalg.init_tensor [128, 64, 96] : tensor<128x64x96xf32>
  %6 = linalg.generic {
    indexing_maps = [ affine_map<(d0, d1, d2) -> (d1, d2, d0)>, affine_map<(d0, d1, d2) -> (d0, d1, d2)>],
    iterator_types = ["parallel", "parallel", "parallel"]}
    ins(%input : tensor<64x96x128xf32>) outs(%output : tensor<128x64x96xf32>) {
    ^bb0(%arg1: f32, %arg2: f32):
      linalg.yield %arg1 : f32
    } -> tensor<128x64x96xf32>
  return %6: tensor<128x64x96xf32>
}

util.global private @"__transpose_210_input" {noinline} = dense<1.0> : tensor<64x96x128xf32>

func @transpose_210() -> tensor<128x96x64xf32> {
  %cst = arith.constant 0.000000e+00 : f32
  %c0 = arith.constant 0 : index
  %input_ptr = util.global.address @"__transpose_210_input" : !util.ptr<tensor<64x96x128xf32>>
  %input = util.global.load.indirect %input_ptr : !util.ptr<tensor<64x96x128xf32>> -> tensor<64x96x128xf32>
  %output = linalg.init_tensor [128, 96, 64] : tensor<128x96x64xf32>
  %6 = linalg.generic {
    indexing_maps = [ affine_map<(d0, d1, d2) -> (d2, d1, d0)>, affine_map<(d0, d1, d2) -> (d0, d1, d2)>],
    iterator_types = ["parallel", "parallel", "parallel"]}
    ins(%input : tensor<64x96x128xf32>) outs(%output : tensor<128x96x64xf32>) {
    ^bb0(%arg1: f32, %arg2: f32):
      linalg.yield %arg1 : f32
    } -> tensor<128x96x64xf32>
  return %6: tensor<128x96x64xf32>
}

util.global private @"__transpose_120_input" {noinline} = dense<1.0> : tensor<64x96x128xf32>

func @transpose_120() -> tensor<96x128x64xf32> {
  %cst = arith.constant 0.000000e+00 : f32
  %c0 = arith.constant 0 : index
  %input_ptr = util.global.address @"__transpose_120_input" : !util.ptr<tensor<64x96x128xf32>>
  %input = util.global.load.indirect %input_ptr : !util.ptr<tensor<64x96x128xf32>> -> tensor<64x96x128xf32>
  %output = linalg.init_tensor [96, 128, 64] : tensor<96x128x64xf32>
  %6 = linalg.generic {
    indexing_maps = [ affine_map<(d0, d1, d2) -> (d2, d0, d1)>, affine_map<(d0, d1, d2) -> (d0, d1, d2)>],
    iterator_types = ["parallel", "parallel", "parallel"]}
    ins(%input : tensor<64x96x128xf32>) outs(%output : tensor<96x128x64xf32>) {
    ^bb0(%arg1: f32, %arg2: f32):
      linalg.yield %arg1 : f32
    } -> tensor<96x128x64xf32>
  return %6: tensor<96x128x64xf32>
}

util.global private @"__transpose_102_input" {noinline} = dense<1.0> : tensor<64x96x128xf32>

func @transpose_102() -> tensor<96x64x128xf32> {
  %cst = arith.constant 0.000000e+00 : f32
  %c0 = arith.constant 0 : index
  %input_ptr = util.global.address @"__transpose_102_input" : !util.ptr<tensor<64x96x128xf32>>
  %input = util.global.load.indirect %input_ptr : !util.ptr<tensor<64x96x128xf32>> -> tensor<64x96x128xf32>
  %output = linalg.init_tensor [96, 64, 128] : tensor<96x64x128xf32>
  %6 = linalg.generic {
    indexing_maps = [ affine_map<(d0, d1, d2) -> (d1, d0, d2)>, affine_map<(d0, d1, d2) -> (d0, d1, d2)>],
    iterator_types = ["parallel", "parallel", "parallel"]}
    ins(%input : tensor<64x96x128xf32>) outs(%output : tensor<96x64x128xf32>) {
    ^bb0(%arg1: f32, %arg2: f32):
      linalg.yield %arg1 : f32
    } -> tensor<96x64x128xf32>
  return %6: tensor<96x64x128xf32>
}
