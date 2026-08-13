#!/usr/bin/env bash
#
# Full clean rebuild for Linux/WSL/macOS: deletes the build directory
# entirely, reconfigures from scratch, builds everything, and runs the
# full test suite. Use this whenever a stale build directory is suspected
# (e.g. an app failing to load QML that was already fixed in the source
# tree, or CMake cache pollution after switching Qt installs) rather than
# a plain `cmake --build`, which only rebuilds targets CMake believes are
# out of date.
#
# Usage:
#   tools/clean_rebuild.sh [build-dir] [-- <extra cmake configure args>]
#
# Examples:
#   tools/clean_rebuild.sh
#   tools/clean_rebuild.sh build
#   tools/clean_rebuild.sh build -- -DCMAKE_PREFIX_PATH=/path/to/Qt/6.10.2/gcc_64
#
# Respects QT_QPA_PLATFORM if already set in the environment (useful for
# headless CI-like runs); otherwise leaves it unset so GUI apps run
# normally when launched by hand afterwards.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${REPO_ROOT}"

BUILD_DIR="build"
if [[ $# -gt 0 && "$1" != "--" ]]; then
    BUILD_DIR="$1"
    shift
fi
if [[ $# -gt 0 && "$1" == "--" ]]; then
    shift
fi
EXTRA_CMAKE_ARGS=("$@")

echo "==> Removing ${BUILD_DIR}/"
rm -rf "${BUILD_DIR}"

echo "==> Configuring (${BUILD_DIR})"
cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug "${EXTRA_CMAKE_ARGS[@]}"

echo "==> Building"
cmake --build "${BUILD_DIR}" --parallel

echo "==> Running full test suite"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

echo "==> Clean rebuild complete: ${BUILD_DIR}"
