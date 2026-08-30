#!/usr/bin/env bash

BUILD_TYPE="${1:-D}"    # D:DEBUG, R:RELEASE
BUILD_ACTION="${2:-B}"  # B:BUILD, C:CLEAN, R:REBUILD

echo "BUILD_TYPE: ${BUILD_TYPE} BUILD_ACTION: ${BUILD_ACTION}"

if [ "${BUILD_TYPE}" = "D" ]; then
    BUILD_PATH="build/Debug"
    LONG_BUILD_TYPE="Debug"
else
    BUILD_PATH="build/Release"
    LONG_BUILD_TYPE="Release"
fi

if [ "${BUILD_ACTION}" = "C" ]; then
    rm -rf ${BUILD_PATH}
elif [ "${BUILD_ACTION}" = "R" ]; then
    rm -rf ${BUILD_PATH}
    cmake -S . -B ${BUILD_PATH} -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake -DCMAKE_BUILD_TYPE=${LONG_BUILD_TYPE} 
else
    cmake -S . -B ${BUILD_PATH} -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake -DCMAKE_BUILD_TYPE=${LONG_BUILD_TYPE}
fi