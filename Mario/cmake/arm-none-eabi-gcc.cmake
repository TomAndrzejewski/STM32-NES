# CMake toolchain definition for STM32CubeIDE

set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set (CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")

# Skip link step during toolchain validation.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Specify toolchain. NOTE When building from inside STM32CubeIDE the location of the toolchain is resolved by the "MCU Toolchain" project setting (via PATH).  
# STM32CubeCLT location: STM32CLT_PATH environment variable if set, otherwise the default install path of the host OS.
if(DEFINED ENV{STM32CLT_PATH})
    file(TO_CMAKE_PATH "$ENV{STM32CLT_PATH}" STM32CLT_ROOT)
elseif(CMAKE_HOST_WIN32)
    set(STM32CLT_ROOT   "C:/ST/STM32CubeCLT_1.22.0")
else()
    set(STM32CLT_ROOT   "/opt/st/stm32cubeclt_1.22.0")
endif()

if(CMAKE_HOST_WIN32)
    set(TOOLCHAIN_SUFFIX ".exe")
else()
    set(TOOLCHAIN_SUFFIX "")
endif()

set(TOOLCHAIN_ROOT      "${STM32CLT_ROOT}/GNU-tools-for-STM32")
set(TOOLCHAIN_PREFIX    "${TOOLCHAIN_ROOT}/bin/arm-none-eabi-")

set(CMAKE_C_COMPILER    "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_ASM_COMPILER  "${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX}")
set(CMAKE_CXX_COMPILER  "${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_SUFFIX}")
set(CMAKE_AR            "${TOOLCHAIN_PREFIX}ar${TOOLCHAIN_SUFFIX}")
set(CMAKE_LINKER        "${TOOLCHAIN_PREFIX}ld${TOOLCHAIN_SUFFIX}")
set(CMAKE_OBJCOPY       "${TOOLCHAIN_PREFIX}objcopy${TOOLCHAIN_SUFFIX}")
set(CMAKE_OBJDUMP       "${TOOLCHAIN_PREFIX}objdump${TOOLCHAIN_SUFFIX}")
set(CMAKE_RANLIB        "${TOOLCHAIN_PREFIX}ranlib${TOOLCHAIN_SUFFIX}")
set(CMAKE_SIZE          "${TOOLCHAIN_PREFIX}size${TOOLCHAIN_SUFFIX}")
set(CMAKE_STRIP         "${TOOLCHAIN_PREFIX}strip${TOOLCHAIN_SUFFIX}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)