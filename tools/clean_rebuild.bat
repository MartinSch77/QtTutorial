@echo off
REM Full clean rebuild for native Windows (cmd.exe): deletes the build
REM directory entirely, reconfigures from scratch, builds everything, and
REM runs the full test suite. Use this whenever a stale build directory is
REM suspected (e.g. an app failing to load QML that was already fixed in
REM the source tree, or CMake cache pollution after switching Qt installs)
REM rather than a plain "cmake --build", which only rebuilds targets CMake
REM believes are out of date.
REM
REM Usage:
REM   tools\clean_rebuild.bat [build-dir] [-- <extra cmake configure args>]
REM
REM Examples:
REM   tools\clean_rebuild.bat
REM   tools\clean_rebuild.bat build
REM   tools\clean_rebuild.bat build -- -DCMAKE_PREFIX_PATH=C:\Qt\6.10.2\msvc2022_64
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "REPO_ROOT=%SCRIPT_DIR%.."
pushd "%REPO_ROOT%" || exit /b 1

set "BUILD_DIR=build"
set "EXTRA_ARGS="
set "PARSING_EXTRA=0"

:parse_args
if "%~1"=="" goto args_done
if "%~1"=="--" (
    set "PARSING_EXTRA=1"
    shift
    goto parse_args
)
if "%PARSING_EXTRA%"=="1" (
    set "EXTRA_ARGS=%EXTRA_ARGS% %~1"
) else (
    set "BUILD_DIR=%~1"
)
shift
goto parse_args
:args_done

echo ==> Removing %BUILD_DIR%\
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"

echo ==> Configuring (%BUILD_DIR%)
cmake -S . -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Debug %EXTRA_ARGS%
if errorlevel 1 goto fail

echo ==> Building
cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 goto fail

echo ==> Running full test suite
ctest --test-dir "%BUILD_DIR%" --output-on-failure
if errorlevel 1 goto fail

echo ==> Clean rebuild complete: %BUILD_DIR%
popd
endlocal
exit /b 0

:fail
echo ==> Clean rebuild FAILED
popd
endlocal
exit /b 1
