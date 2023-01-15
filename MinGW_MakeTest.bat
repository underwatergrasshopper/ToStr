:: Runs specific unit tests (MinGW).

:: MinGW_MakeTest <action> <mode> <architecture>
::      <action>
::          build
::          run
::      <mode>
::          Debug
::          DebugWIDE
::          Release
::          ReleaseWIDE
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

set ERR_PASS=0

cd !TEST_PROJECT_NAME!
call MinGW_Make.bat %*
if !ERRORLEVEL! neq 0 set ERR_PASS=!ERRORLEVEL!
cd ..
if !ERR_PASS! neq 0 exit /B !ERR_PASS!
exit /B