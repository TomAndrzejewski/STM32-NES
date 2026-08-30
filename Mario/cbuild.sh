#!/usr/bin/env bash

BUILD_TYPE="${1:-D}"    # D:DEBUG, R:RELEASE
BUILD_ACTION="${2:-B}"  # B:BUILD, C:CLEAN, R:REBUILD

if [ "${BUILD_TYPE}" = "D" ]; then
    BUILD_PATH="build/Debug"
else
    BUILD_PATH="build/Release"
fi

if [ "${BUILD_ACTION}" = "C" ]; then
    cmake --build ${BUILD_PATH} --target clean -j8
elif [ "${BUILD_ACTION}" = "R" ]; then
    cmake --build ${BUILD_PATH} --target clean -j8
    cmake --build ${BUILD_PATH} -j8
else
    cmake --build ${BUILD_PATH} -j8
fi