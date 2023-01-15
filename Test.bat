:: Runs all unit tests (Visual Studio).
@echo off
setlocal EnableDelayedExpansion

::------------------------------------------------------------------------------
:: User Section

set SLN_FILE=ToStr.sln
set TEST_EXECUTABLE=ToStr_Test.exe
set TEST_PROJECT_NAME=ToStr_Test

set NO_FLAG_INDICATOR=NO_FLAG

:: each parameter in set is separated by +, later + is replaced by spaces when set of flags is forwarded as parameters to test executable
set FLAG_SET_LIST=NO_FLAG

set PLATFORM_LIST=x86 x64
set CONFIGURATION_LIST=Debug Release

::------------------------------------------------------------------------------

if not exist ".\\TestCache.bat" (
    echo set MSBUILD_PATH=
) > TestCache.bat

call .\\TestCache.bat

if "%MSBUILD_PATH%" equ "" goto :SKIP_ADD_MSBUILD_PATH
set PATH=%PATH%;%MSBUILD_PATH%
:SKIP_ADD_MSBUILD_PATH

set WORKING_DIRECTORY=%CD%

if exist ".\\build" rd /S /Q ".\\build" 

:: Build
for %%P in (!PLATFORM_LIST!) do (
    set PLATFORM=%%P
    for %%C in (!CONFIGURATION_LIST!) do (
        set CONFIGURATION=%%C
        
        set COMMAND=MSBuild !SLN_FILE! /t:!TEST_PROJECT_NAME! /p:Configuration=!CONFIGURATION! /p:Platform=!PLATFORM!
        
        echo.
        echo Command ^(!CONFIGURATION!, !PLATFORM!^): !COMMAND!
        
        !COMMAND!
        if !ERRORLEVEL! neq 0 (
            echo === TEST FAILURE ^(!CONFIGURATION!, !PLATFORM!^) ===
            exit /B !ERRORLEVEL!
        )
    ) 
)

:: Run Test
for %%P in (!PLATFORM_LIST!) do (
    set PLATFORM=%%P
    for %%C in (!CONFIGURATION_LIST!) do (
        set CONFIGURATION=%%C

        set PLATFORM_FOLDER=Win32
        if "!PLATFORM!" equ "x64" set PLATFORM_FOLDER=x64
        
        set OUT_DIR=build\\vs\\!PLATFORM_FOLDER!\\!CONFIGURATION!

        if not exist ".\\!OUT_DIR!" (
            echo There is no ".\\!OUT_DIR!" folder.
            echo === TEST FAILURE ^(!CONFIGURATION!, !PLATFORM!^) ===
            exit /B 1
        )

        cd !OUT_DIR!

        if not exist ".\\!TEST_EXECUTABLE!" (
            echo There is no ".\\!TEST_EXECUTABLE!" executable.
            echo === TEST FAILURE ^(!CONFIGURATION!, !PLATFORM!^) ===
            exit /B 1
        )
        
        for %%F in (!FLAG_SET_LIST!) do (
            set FLAG_SET=%%F
            set FLAG_SET=!FLAG_SET:+= !
            if "!FLAG_SET!" equ "!NO_FLAG_INDICATOR!" set FLAG_SET=
            
            set COMMAND=!TEST_EXECUTABLE! !FLAG_SET!
            
            echo.
            echo Command ^(!CONFIGURATION!, !PLATFORM!^): !COMMAND!
            
            !COMMAND!
            if !ERRORLEVEL! neq 0 (
                echo === TEST FAILURE ^(!CONFIGURATION!, !PLATFORM!^) ===
                exit /B !ERRORLEVEL!
            )
        )

        cd !WORKING_DIRECTORY!
    ) 
)

echo === TEST SUCCESS ===
exit /B
