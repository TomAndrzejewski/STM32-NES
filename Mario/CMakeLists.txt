cmake_minimum_required(VERSION 3.20)

project(Mario
    LANGUAGES C ASM
)

# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LINKER_SCRIPT
    ${CMAKE_SOURCE_DIR}/STM32F446RETX_FLASH.ld
)

# -----------------------------------------------------------------------------
# Sources
# -----------------------------------------------------------------------------

file(GLOB MY_SOURCES "Sources/Src/*.c")

list(APPEND MY_SOURCES
    ${CMAKE_SOURCE_DIR}/Startup/startup_stm32f446retx.s
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
    -Wno-switch

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
    ${CMAKE_SOURCE_DIR}/Sources/Inc
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