@echo off

echo RunTestGCC Preparing ...
if not exist "Test" mkdir "Test"

echo RunTestGCC Clearing ...
set OUT_EXE=Test\\ToStrGCC.exe
if exist "%OUT_EXE%" del "%OUT_EXE%"

echo RunTestGCC Compiling ...
set COMMAND_LINE=g++ -std=c++11 -O3 -D TOSTR_TEST -I .\\include tests\\*.cpp -o %OUT_EXE%
echo %COMMAND_LINE%
call %COMMAND_LINE%

echo RunTestGCC Running ...
if exist "%OUT_EXE%" (
    call %OUT_EXE%
) else (
    echo RunTestGCC Error: %OUT_EXE% does not exist.
)