#!/bin/bash -e

mkdir -p DIST/llvm

cmake -S llvm -B build -G Ninja \
    -DLLVM_ENABLE_PROJECTS="clang;lld;clang-tools-extra" \
    -DLLVM_BUILD_RUNTIMES=OFF \
    -DCMAKE_INSTALL_PREFIX=`pwd`"/DIST/llvm" \
    -DCMAKE_BUILD_TYPE="Release" \
    -DLLVM_TARGETS_TO_BUILD=X86 \
    -DCMAKE_CXX_STANDARD=20

cmake --build build -- -j5
cmake --build build -- install

cp BUILD.go DIST/llvm
