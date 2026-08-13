#!/usr/bin/env bash
#
# Local quality gate a contributor can run before opening a PR. Mirrors (a
# scaled-down, open-source-only subset of) the checks the CI workflows under
# .github/workflows/ run: configure+build, ctest, cppcheck, clang-tidy, and
# the requirements traceability report.
#
# Does NOT run sanitizer or coverage builds (those use different CMake
# configure flags than a normal Debug build) or clazy (needs a compile
# database the same as clang-tidy but is left out here to keep a local run
# fast; see .github/workflows/static-analysis.yml for that one). Run those
# separately if you're touching sanitizer- or coverage-sensitive code.
#
# Usage: tools/run_quality_checks.sh [build-dir]
#   build-dir defaults to "build" under the repo root.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-${REPO_ROOT}/build}"

cd "${REPO_ROOT}"

# Track pass/fail per step so we can print an honest summary at the end even
# though `set -e` would otherwise abort on the first failure. We deliberately
# still exit non-zero overall if anything failed.
declare -A STEP_STATUS
OVERALL_STATUS=0

run_step() {
    local name="$1"
    shift
    echo ""
    echo "==> ${name}"
    if "$@"; then
        STEP_STATUS["${name}"]="PASS"
    else
        STEP_STATUS["${name}"]="FAIL"
        OVERALL_STATUS=1
    fi
}

configure() {
    cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
}

build() {
    cmake --build "${BUILD_DIR}" --parallel
}

run_ctest() {
    ctest --test-dir "${BUILD_DIR}" --output-on-failure
}

run_cppcheck() {
    if ! command -v cppcheck >/dev/null 2>&1; then
        echo "cppcheck not found on PATH, skipping." >&2
        return 0
    fi
    # "style" (e.g. useStlAlgorithm) is genuinely subjective and, unlike
    # warning/performance/portability, is not gated here -- it's printed
    # (informational, matching .github/workflows/static-analysis.yml's
    # continue-on-error stance for the same category) but doesn't fail the
    # gate on its own.
    cppcheck \
        --enable=style \
        --std=c++23 \
        --library=qt \
        --inline-suppr \
        -i "${BUILD_DIR}" \
        "${REPO_ROOT}/framework-tour" \
        "${REPO_ROOT}/industries" || true
    cppcheck \
        --enable=warning,performance,portability \
        --std=c++23 \
        --library=qt \
        --error-exitcode=1 \
        --inline-suppr \
        -i "${BUILD_DIR}" \
        "${REPO_ROOT}/framework-tour" \
        "${REPO_ROOT}/industries"
}

run_clang_tidy() {
    local compdb="${BUILD_DIR}/compile_commands.json"
    if [[ ! -f "${compdb}" ]]; then
        echo "No compile_commands.json at ${compdb}, skipping clang-tidy." >&2
        return 0
    fi
    if command -v run-clang-tidy >/dev/null 2>&1; then
        run-clang-tidy -p "${BUILD_DIR}" -quiet \
            '^(framework-tour|industries)/.*\.(cpp|cc)$'
    elif command -v clang-tidy >/dev/null 2>&1; then
        echo "run-clang-tidy not found, falling back to per-file clang-tidy." >&2
        local failed=0
        while IFS= read -r -d '' src; do
            clang-tidy -p "${BUILD_DIR}" "${src}" || failed=1
        done < <(find "${REPO_ROOT}/framework-tour" "${REPO_ROOT}/industries" \
                      -name '*.cpp' -print0)
        return "${failed}"
    else
        echo "Neither run-clang-tidy nor clang-tidy found on PATH, skipping." >&2
        return 0
    fi
}

run_trace_report() {
    python3 "${REPO_ROOT}/tools/trace_report.py"
}

run_step "configure" configure
run_step "build" build
run_step "ctest" run_ctest
run_step "cppcheck" run_cppcheck
run_step "clang-tidy" run_clang_tidy
run_step "trace_report" run_trace_report

echo ""
echo "===================== Quality Check Summary ====================="
for name in "configure" "build" "ctest" "cppcheck" "clang-tidy" "trace_report"; do
    printf "  %-14s %s\n" "${name}" "${STEP_STATUS[${name}]:-SKIPPED}"
done
echo "===================================================================="

if [[ "${OVERALL_STATUS}" -eq 0 ]]; then
    echo "Overall: PASS"
else
    echo "Overall: FAIL"
fi

exit "${OVERALL_STATUS}"
