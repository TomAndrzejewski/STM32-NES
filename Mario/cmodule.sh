#!/usr/bin/env bash

# Creates <Module>.h and <Module>.c skeletons following the file layout from CLAUDE.md.
# Usage: ./cmodule.sh <layer> <ModuleName> [-f]
#   layer:      app | common | drivers | game | platform | thirdparty
#   ModuleName: e.g. BatteryControl (file names and function prefix are derived from it)
#   -f:         overwrite existing files

LAYER="${1}"
MODULE="${2}"
FORCE="${3}"

if [ -z "${LAYER}" ] || [ -z "${MODULE}" ]; then
    echo "Usage: $0 <app|common|drivers|game|platform|thirdparty> <ModuleName> [-f]"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LAYER_DIR="${SCRIPT_DIR}/sources/${LAYER}"

if [ ! -d "${LAYER_DIR}" ]; then
    echo "Unknown layer: ${LAYER} (no directory ${LAYER_DIR})"
    exit 1
fi

if ! [[ "${MODULE}" =~ ^[A-Za-z][A-Za-z0-9_]*$ ]]; then
    echo "Invalid module name: ${MODULE} (letters, digits and _ only, must start with a letter)"
    exit 1
fi

H_FILE="${LAYER_DIR}/inc/${MODULE}.h"
C_FILE="${LAYER_DIR}/src/${MODULE}.c"

# Refuse to overwrite existing files unless forced
if [ "${FORCE}" != "-f" ]; then
    for f in "${H_FILE}" "${C_FILE}"; do
        if [ -e "${f}" ]; then
            echo "File exists: ${f} (use -f to overwrite)"
            exit 1
        fi
    done
fi

# Header date in the same format as the existing files, e.g. "27 paz 2026"
MONTHS=(sty lut mar kwi maj cze lip sie wrz paz lis gru)
DATE="$(date +%-d) ${MONTHS[$(( $(date +%-m) - 1 ))]} $(date +%Y)"
AUTHOR="${USER:-$(whoami)}"
GUARD="SOURCES_INC_$(echo "${MODULE}" | tr '[:lower:]' '[:upper:]')_H_"

mkdir -p "${LAYER_DIR}/inc" "${LAYER_DIR}/src"

cat > "${H_FILE}" << EOF
/*
 * ${MODULE}.h
 *
 *  Created on: ${DATE}
 *      Author: ${AUTHOR}
 */

#ifndef ${GUARD}
#define ${GUARD}



#endif // ${GUARD}
EOF

cat > "${C_FILE}" << EOF
/*
 * ${MODULE}.c
 *
 *  Created on: ${DATE}
 *      Author: ${AUTHOR}
 */

#include "${MODULE}.h"

#include <stddef.h>


//----------------
// PRIVATE MACROS
//----------------


//----------------
// PRIVATE TYPES
//----------------


//----------------
// PRIVATE VARIABLES
//----------------


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------


//----------------
// PUBLIC FUNCTIONS
//----------------


//----------------
// PRIVATE FUNCTIONS
//----------------

EOF

echo "Created: ${H_FILE}"
echo "Created: ${C_FILE}"
echo "Re-run ./ctarget.sh so CMake picks up the new .c file."
