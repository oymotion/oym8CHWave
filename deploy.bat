@echo off
setlocal enabledelayedexpansion

set "QT_ROOT=C:\Qt"
set "QT_VER=6.*"

:: Find latest Qt version by modification date
set "QT_DIR="
for /f "delims=" %%i in ('dir /B /A:D /O-D "%QT_ROOT%\%QT_VER%" 2^>nul') do (
    set "QT_DIR=%%i"
    goto :break_qt
)
:break_qt
if not defined QT_DIR (
    echo Error: No Qt version found in %QT_ROOT%\%QT_VER%
    goto :done
)
set "QT_BIN_DIR=%QT_ROOT%\%QT_DIR%\msvc2022_64\bin"
echo Using Qt bin dir: %QT_BIN_DIR%

:: Find latest MinGW version by modification date
set "MINGW_TOOLS_DIR="
for /f "delims=" %%i in ('dir /B /A:D /O-D "%QT_ROOT%\Tools\mingw*_64" 2^>nul') do (
    set "MINGW_TOOLS_DIR=%%i"
    goto :break_mingw
)
:break_mingw
if not defined MINGW_TOOLS_DIR (
    echo Error: No MinGW directory found in %QT_ROOT%\Tools
    goto :done
)
set "MINGW_DIR=%QT_ROOT%\Tools\%MINGW_TOOLS_DIR%\bin"
echo Using MinGW dir: %MINGW_DIR%

:: Configure paths and parameters
set "PATH=%QT_BIN_DIR%;%MINGW_DIR%;%PATH%"
set "DEPLOY=%QT_BIN_DIR%\windeployqt.exe"
set "DEPLOY_ARGS=--compiler-runtime --include-soft-plugins --no-quick-import --no-opengl-sw --no-system-d3d-compiler --no-system-dxc-compiler --no-quick-import"
set "EXE=oym8CHWave.exe"
@REM set "EXTRA=%MINGW_DIR%\libgcc_s_seh-*.dll %MINGW_DIR%\libstdc++-*.dll %MINGW_DIR%\libwinpthread-*.dll oym8CHWave\third_party\gforce\gforce32.dll oym8CHWave\third_party\gforce\gforce64.dll"
set "EXTRA=oym8CHWave\third_party\gforce\gforce32.dll oym8CHWave\third_party\gforce\gforce64.dll"
set "SRC_DIR=oym8CHWave"
set "SUB_DIR=%SRC_DIR%\build"
set "DIST_DIR=oym8CHWave_dist"

:: Find build directory
set "BUILD_DIR="
for /f "delims=" %%i in ('dir /B /A:D "%SUB_DIR%\*MSVC*Release" 2^>nul') do (
    set "candidate=%SUB_DIR%\%%i\release"
    if exist "!candidate!\%EXE%" (
        set "BUILD_DIR=!candidate!"
        goto :found_build
    )
)

if not defined BUILD_DIR (
    echo Error: "%EXE%" not found in any build directory
    echo Check compilation in %SUB_DIR%\*MSVC*Release\release
    goto :done
)

:found_build
echo Using build dir: %BUILD_DIR%

:: Prepare distribution directory
if not exist "%DIST_DIR%" mkdir %DIST_DIR%
@REM if not exist "%DIST_DIR%\transpations" mkdir %DIST_DIR%\transpations

:: Copy main executable
echo Copying main executable...
copy /Y "%BUILD_DIR%\%EXE%" "%DIST_DIR%\"

:: Copy required DLLs
echo Copying required DLLs...
for %%x in (%EXTRA%) do copy /Y "%%x" "%DIST_DIR%\" >nul

:: Run deployment tool
echo Running windeployqt...
"%DEPLOY%" %DEPLOY_ARGS% "%DIST_DIR%\%EXE%"

:: Copy translation files
echo Copying user translation files...
for /f "delims=" %%i in ('dir /B /S /O-D "%SRC_DIR%\*.qm" 2^>nul') do (
    @echo copying "%%i"...
    @copy /Y "%%i" "%DIST_DIR%\translations\" >nul
)

echo.
echo Deployment completed successfully!

:done
echo.
pause
endlocal
