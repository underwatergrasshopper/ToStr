:: Packs code into library archive to distribution.
@echo off
setlocal EnableDelayedExpansion

::------------------------------------------------------------------------------
:: User Section

set VERSION=0.1.0
set NAME=ToStr

::------------------------------------------------------------------------------

set FILES=README.md CHANGELOG.md LICENSE

where /q 7z.exe || echo Pack Error: Can not find 7z.exe. && exit /b

set NAME_VERSION=%NAME%-%VERSION%
set INCLUDE_FOLDER=include

if not exist "dist" mkdir "dist"

echo Clean %NAME_VERSION%
if exist "dist\\%NAME_VERSION%.zip" del "dist\\%NAME_VERSION%.zip"
if exist "dist\\%NAME%" rd /S /Q "dist\\%NAME%"
echo.

echo Pack %NAME_VERSION%
mkdir "dist\\%NAME%"
for %%F in (%FILES%) do xcopy %%F "dist\\%NAME%" /I
if "%INCLUDE_FOLDER%" neq "" (
    xcopy %INCLUDE_FOLDER% "dist\\%NAME%\\include" /I
)

cd dist
call 7z.exe a "%NAME_VERSION%.zip" "%NAME%"
cd ..

if exist "dist\\%NAME%" rd /S /Q "dist\\%NAME%"