:: Runs all unit tests (MinGW).
@echo off
setlocal EnableDelayedExpansion

::------------------------------------------------------------------------------
:: User Section

set NO_FLAG_INDICATOR=NO_FLAG

:: each parameter in set is separated by +, later + is replaced by spaces when set of flags is forwarded as parameters to test executable
set FLAG_SET_LIST=NO_FLAG

set ARCHITECTURE_LIST=32 64
set BUILD_MODE_LIST=Debug Release

::------------------------------------------------------------------------------

for %%P in (!ARCHITECTURE_LIST!) do (
    set ARCHITECTURE=%%P
    for %%C in (!BUILD_MODE_LIST!) do (
        set BUILD_MODE=%%C
        
        for %%F in (!FLAG_SET_LIST!) do (
            set FLAG_SET=%%F
            set FLAG_SET=!FLAG_SET:+= !
            if "!FLAG_SET!" equ "!NO_FLAG_INDICATOR!" set FLAG_SET=
            
            set COMMAND=call .\\MinGW_MakeTest.bat run !BUILD_MODE! !ARCHITECTURE! !FLAG_SET!
            
            echo.
            echo Command ^(!BUILD_MODE!, !ARCHITECTURE!^): !COMMAND!
            
            !COMMAND!
            if !ERRORLEVEL! neq 0 (
                echo === TEST FAILURE ^(!BUILD_MODE!, !ARCHITECTURE!^) ===
                exit /B !ERRORLEVEL!
            )
        )
    )
)

echo === TEST SUCCESS ===
exit /B