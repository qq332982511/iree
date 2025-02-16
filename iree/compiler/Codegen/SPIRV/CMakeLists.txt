################################################################################
# Autogenerated by build_tools/bazel_to_cmake/bazel_to_cmake.py from           #
# iree/compiler/Codegen/SPIRV/BUILD                                            #
#                                                                              #
# Use iree_cmake_extra_content from iree/build_defs.oss.bzl to add arbitrary   #
# CMake-only content.                                                          #
#                                                                              #
# To disable autogeneration for this file entirely, delete this header.        #
################################################################################

iree_add_all_subdirs()

iree_cc_library(
  NAME
    SPIRV
  HDRS
    "KernelConfig.h"
    "MemorySpace.h"
    "Utils.h"
  SRCS
    "AdrenoConfig.cpp"
    "ConvertToSPIRVPass.cpp"
    "KernelConfig.cpp"
    "MaliConfig.cpp"
    "NVIDIAConfig.cpp"
    "Passes.cpp"
    "SPIRVCreateFastSlowPath.cpp"
    "SPIRVDistribute.cpp"
    "SPIRVFuseTensorPadWithConsumer.cpp"
    "SPIRVLowerExecutableTargetPass.cpp"
    "SPIRVTile.cpp"
    "SPIRVTileAndDistribute.cpp"
    "SPIRVTileAndVectorizeToCooperativeOps.cpp"
    "SPIRVVectorToCooperativeOps.cpp"
    "SPIRVVectorize.cpp"
    "SPIRVVectorizeLoadStore.cpp"
    "SPIRVVectorizePad.cpp"
    "Utils.cpp"
  DEPS
    IREELinalgExtDialect
    IREELinalgExtPasses
    LLVMSupport
    MLIRAffine
    MLIRAffineAnalysis
    MLIRAffineToStandard
    MLIRAffineUtils
    MLIRAnalysis
    MLIRArithmetic
    MLIRArithmeticToSPIRV
    MLIRArithmeticTransforms
    MLIRArithmeticUtils
    MLIRBufferization
    MLIRControlFlowToSPIRV
    MLIRFunc
    MLIRFuncToSPIRV
    MLIRFuncTransforms
    MLIRGPUOps
    MLIRGPUToSPIRV
    MLIRGPUTransforms
    MLIRIR
    MLIRLinalg
    MLIRLinalgTransforms
    MLIRMathToSPIRV
    MLIRMemRef
    MLIRMemRefToSPIRV
    MLIRMemRefTransforms
    MLIRPass
    MLIRSCF
    MLIRSCFToGPU
    MLIRSCFToSPIRV
    MLIRSCFTransforms
    MLIRSCFUtils
    MLIRSPIRV
    MLIRSPIRVConversion
    MLIRSPIRVTransforms
    MLIRSideEffectInterfaces
    MLIRSupport
    MLIRTensor
    MLIRTensorToSPIRV
    MLIRTensorTransforms
    MLIRTosa
    MLIRTosaToArith
    MLIRTransforms
    MLIRVector
    MLIRVectorInterfaces
    MLIRVectorToSPIRV
    MLIRVectorTransforms
    iree::compiler::Codegen::Common
    iree::compiler::Codegen::Dialect::IREECodegenDialect
    iree::compiler::Codegen::PassHeaders
    iree::compiler::Codegen::Transforms
    iree::compiler::Codegen::Utils
    iree::compiler::Dialect::Flow::IR
    iree::compiler::Dialect::HAL::IR
    iree::compiler::Dialect::HAL::IR::HALDialect
    iree::compiler::Dialect::Util::IR
  PUBLIC
)

### BAZEL_TO_CMAKE_PRESERVES_ALL_CONTENT_BELOW_THIS_LINE ###
