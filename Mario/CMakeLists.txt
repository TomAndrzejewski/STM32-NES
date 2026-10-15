cmake_minimum_required(VERSION 3.20)

project(Mario
    LANGUAGES C ASM
)

# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LINKER_SCRIPT
    ${CMAKE_SOURCE_DIR}/sources/platform/src/STM32F446RETX_FLASH.ld
)

# -----------------------------------------------------------------------------
# Sources
# -----------------------------------------------------------------------------

file(GLOB MY_SOURCES    "sources/app/*.c"
                        "sources/game/src/*.c"
                        "sources/common/src/*.c"
                        "sources/drivers/src/*.c"
                        "sources/platform/src/*.c"
                        "sources/thirdparty/src/*.c")

list(APPEND MY_SOURCES
    ${CMAKE_SOURCE_DIR}/sources/platform/src/startup_stm32f446retx.s
)

# -----------------------------------------------------------------------------
# Target
# -----------------------------------------------------------------------------

add_executable(${PROJECT_NAME}.elf 
    ${MY_SOURCES}
)

# -----------------------------------------------------------------------------
# MCU / compiler options
# -----------------------------------------------------------------------------

target_compile_options(${PROJECT_NAME}.elf PRIVATE

    # MCU
    -mcpu=cortex-m4
    -mthumb

    # FPU
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard

    # C
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>

    # Warnings
    -Wall
    -Wextra
    -Wno-switch
    -Wdouble-promotion
    -Wfloat-conversion
    -Werror=override-init
    #-Wshadow

    # Debug / Release
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-O2>
)

# -----------------------------------------------------------------------------
# Preprocessor definitions
# -----------------------------------------------------------------------------

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE

    $<$<CONFIG:Debug>:DEBUG>
)

# -----------------------------------------------------------------------------
# Include directories
# -----------------------------------------------------------------------------

# Your own headers
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/sources/app/inc
    ${CMAKE_SOURCE_DIR}/sources/game/inc
    ${CMAKE_SOURCE_DIR}/sources/common/inc
    ${CMAKE_SOURCE_DIR}/sources/drivers/inc
    ${CMAKE_SOURCE_DIR}/sources/platform/inc
    ${CMAKE_SOURCE_DIR}/sources/thirdparty/inc
)

# External/system headers: CMSIS, STM32 device headers, LL.
# Marked as SYSTEM so clangd/compiler treats them as system headers.
target_include_directories(${PROJECT_NAME}.elf SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/ThirdParty/CMSIS
    ${CMAKE_SOURCE_DIR}/ThirdParty/Device
    ${CMAKE_SOURCE_DIR}/ThirdParty/Device/LowLayer
)

# -----------------------------------------------------------------------------
# Linker
# -----------------------------------------------------------------------------

target_link_options(${PROJECT_NAME}.elf PRIVATE

    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard

    -T${LINKER_SCRIPT}

    --specs=nano.specs
    --specs=nosys.specs

    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map

    -static
    -Wl,--start-group
    -lc
    -lm
    -Wl,--end-group
)

# -----------------------------------------------------------------------------
# Output files
# -----------------------------------------------------------------------------

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD

    # Size
    COMMAND ${CMAKE_SIZE}
            $<TARGET_FILE:${PROJECT_NAME}.elf>

    # HEX
    COMMAND ${CMAKE_OBJCOPY}
            -O ihex
            $<TARGET_FILE:${PROJECT_NAME}.elf>
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex

    # Assembly listing
    COMMAND ${CMAKE_OBJDUMP}
            -h -S
            $<TARGET_FILE:${PROJECT_NAME}.elf>
            > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.list

    COMMENT "Generating HEX and assembly listing"
)