#!/usr/bin/env bash
#
# Illustrative Squish Coco instrumentation for this CMake project. NOT run
# by this repo's CI (no Coco license here) -- see ../../docs/qa/licensed-tools.md
# and README.md in this directory. Adapted from Coco's documented
# CoverageScanner-as-compiler-wrapper usage pattern; not verified against a
# real Coco installation, since none is available in this environment.
#
# Usage: tool-configs/coco/coco_build_example.sh [build-dir]

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${1:-${REPO_ROOT}/build-coco}"

# CoverageScanner ships wrapper compilers (csgcc/csg++, or cscl/cscc on
# Windows) that instrument every translation unit for statement + MC/DC
# coverage as it compiles, writing a .csmes coverage database alongside
# the build. Point CMake's compilers at the wrappers instead of the real
# ones:
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=csgcc \
    -DCMAKE_CXX_COMPILER=csg++ \
    -DCMAKE_CXX_FLAGS="--cs-mcdc --cs-on"

cmake --build "${BUILD_DIR}" --parallel 2

ctest --test-dir "${BUILD_DIR}" --output-on-failure

# csreport (Coco's report generator) turns the accumulated .csmes into an
# HTML report with per-decision MC/DC coverage -- the specific gap this
# repo's open-source gcovr-based coverage.yml workflow cannot fill (gcov
# reports line/branch coverage, not MC/DC).
csreport --html --title "QtTutorial (illustrative Coco run)" \
    -o "${BUILD_DIR}/coco-report" "${BUILD_DIR}"/*.csmes
