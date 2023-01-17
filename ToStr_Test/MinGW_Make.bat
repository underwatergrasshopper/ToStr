:: Builds project by using MinGW GCC Compiler and/or runs built application.

:: MinGW_Make <action> <mode> <architecture>
::      <action>
::          build
::          run
::          just_run
::      <mode>
::          Debug
::          Release
::      <architecture>
::          32
::          64
::      <test_flags>
::          <word>( <word>)*

@echo off
setlocal EnableDelayedExpansion

::------------------------------------------------------------------------------
:: User Section

set TEST_PROJECT_NAME=ToStr_Test

::------------------------------------------------------------------------------

:: <none>, build, rebuild, clean, run
set ACTION=%1
if "%ACTION%" equ "" set ACTION=run

:: <none>, Debug, Release
set BUILD_TYPE=%2
if "%BUILD_TYPE%" equ "" set BUILD_TYPE=Release

:: <none>, 32, 64
set ARCHITECTURE=%3
if "%ARCHITECTURE%" equ "" set ARCHITECTURE=64

:: <none>, <word>( <word>)*
set TEST_FLAGS=
call :FETCH_TEST_FLAGS %*

if not exist .\\MinGW_MakeCache.bat (
    echo set MINGW32_BIN_PATH=
    echo set MINGW64_BIN_PATH=
) > MinGW_MakeCache.bat 

call .\\MinGW_MakeCache.bat

if "%MINGW32_BIN_PATH%" equ "" goto :SKIP_MINGW32_BIN_PATH
if "%ARCHITECTURE%" neq "32" goto :SKIP_MINGW32_BIN_PATH
set PATH=%MINGW32_BIN_PATH%;%PATH%
:SKIP_MINGW32_BIN_PATH

if "%MINGW64_BIN_PATH%" equ "" goto :SKIP_MINGW64_BIN_PATH
if "%ARCHITECTURE%" neq "64" goto :SKIP_MINGW64_BIN_PATH
set PATH=%MINGW64_BIN_PATH%;%PATH%
:SKIP_MINGW64_BIN_PATH

set PROJECT_FOLDER=
for %%I in (.) do set PROJECT_FOLDER=%%~nxI

set EXE_FILE_NAME=.\\!PROJECT_FOLDER!.exe

set IS_OK=False
if "!BUILD_TYPE!" equ "Debug" set IS_OK=True
if "!BUILD_TYPE!" equ "Release" set IS_OK=True

set ARCH_PRE=
if "!ARCHITECTURE!" equ "64" set ARCH_PRE=x64
if "!ARCHITECTURE!" equ "32" set ARCH_PRE=Win32

set BUILD_SUB_DIR=..\\build\\mingw_llvm
set BUILD_PATH=!BUILD_SUB_DIR!\\!ARCH_PRE!\\!BUILD_TYPE!
set RETURN_PATH=..\\..\\..\\..\\!TEST_PROJECT_NAME!

set ERR_PASS=0

if "!IS_OK!" equ "True" (
    if "!ACTION!" equ "build" (
        call :BUILD
        if !ERRORLEVEL! neq 0 exit /B !ERRORLEVEL!
    ) else if "!ACTION!" equ "rebuild" (
        call :CLEAN
        call :BUILD
        if !ERRORLEVEL! neq 0 exit /B !ERRORLEVEL!
    ) else if "!ACTION!" equ "clean" (
        call :CLEAN
    ) else if "!ACTION!" equ "clean_all" (
        call :CLEAN_ALL
    ) else if "!ACTION!" equ "run" (
        call :BUILD
        if !ERRORLEVEL! neq 0 exit /B !ERRORLEVEL!
        call :RUN
        if !ERRORLEVEL! neq 0 exit /B !ERRORLEVEL!
    ) else if "!ACTION!" equ "just_run" (
        call :RUN
        if !ERRORLEVEL! neq 0 exit /B !ERRORLEVEL!
    ) else (
        echo Run Error: Unknown action type: "!ACTION!".
        exit /B 1
    )
    
) else (
    echo Run Error: Unknown build type: "!BUILD_TYPE!".
    exit /B 1
)

goto :EOF

:CLEAN
    if exist .\\!BUILD_PATH! @rd /S /Q .\\!BUILD_PATH!
    exit /B

:CLEAN_ALL
    if exist .\\!BUILD_SUB_DIR! @rd /S /Q .\\!BUILD_SUB_DIR!
    exit /B

:BUILD
    if not exist !BUILD_PATH! md !BUILD_PATH!
    set BUILD_PATH=!BUILD_PATH:\\=/!
    cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=!BUILD_TYPE! -D  ARCHITECTURE=!ARCHITECTURE! -S . -B !BUILD_PATH! && cmake --build !BUILD_PATH!
    if !ERRORLEVEL! neq 0 exit /B !ERRORLEVEL!
    exit /B

:RUN
    if not exist !BUILD_PATH! md !BUILD_PATH!
    cd !BUILD_PATH!
    !EXE_FILE_NAME! !TEST_FLAGS!
    if !ERRORLEVEL! neq 0 set ERR_PASS=!ERRORLEVEL!
    cd !RETURN_PATH!
    if !ERR_PASS! neq 0 exit /B !ERR_PASS!

    exit /B
    
:FETCH_TEST_FLAGS
    set TEST_FLAGS=
    :FETCH_TEST_FLAG_LOOP
        if "%4" equ "" goto :END_FETCH_TEST_FLAGS
        set TEST_FLAGS=!TEST_FLAGS! %4
        shift
        goto :FETCH_TEST_FLAG_LOOP
    :END_FETCH_TEST_FLAGS
    exit /B

:EOF
