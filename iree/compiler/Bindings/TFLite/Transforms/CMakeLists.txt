################################################################################
# Autogenerated by build_tools/bazel_to_cmake/bazel_to_cmake.py from           #
# iree/compiler/Bindings/TFLite/Transforms/BUILD                               #
#                                                                              #
# Use iree_cmake_extra_content from iree/build_defs.oss.bzl to add arbitrary   #
# CMake-only content.                                                          #
#                                                                              #
# To disable autogeneration for this file entirely, delete this header.        #
################################################################################

iree_add_all_subdirs()

iree_cc_library(
  NAME
    Transforms
  HDRS
    "Passes.h"
  SRCS
    "Passes.cpp"
    "WrapEntryPoints.cpp"
  DEPS
    LLVMSupport
    MLIRAffineUtils
    MLIRControlFlow
    MLIRFunc
    MLIRIR
    MLIRPass
    MLIRShape
    MLIRShapeOpsTransforms
    MLIRSupport
    MLIRTensor
    MLIRTransformUtils
    MLIRTransforms
    iree::compiler::Dialect::Flow::IR
    iree::compiler::Dialect::HAL::IR
    iree::compiler::Dialect::HAL::IR::HALDialect
    iree::compiler::Dialect::Util::IR
    iree::compiler::Utils
  PUBLIC
)

### BAZEL_TO_CMAKE_PRESERVES_ALL_CONTENT_BELOW_THIS_LINE ###
