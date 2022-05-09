#!/bin/bash
mkdir -p DIST/llvm

cmake -S llvm -B build -G "Unix Makefiles" \
    -DLLVM_ENABLE_PROJECTS="clang;lld;clang-tools-extra" \
    -DLLVM_BUILD_RUNTIMES=OFF \
    -DCMAKE_INSTALL_PREFIX=`pwd`"/DIST/llvm" \
    -DCMAKE_BUILD_TYPE="Release" \
    -DLLVM_TARGETS_TO_BUILD=X86 \
    -DCMAKE_CXX_STANDARD=20
 
    #-DLIBCLANG_BUILD_STATIC=ON \
    #-DLLVM_STATIC_LINK_CXX_STDLIB=ON \
    #-DBUILD_SHARED_LIBS=OFF
    #-DLLVM_BUILD_STATIC=ON \
    #-DLLVM_ENABLE_RUNTIMES="compiler-rt" \

cd build
make -j5
make install
cd ..
cp BUILD.go DIST/llvm
