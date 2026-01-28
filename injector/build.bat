@echo off
setlocal enabledelayedexpansion

echo [INFO] Attempting to build Injector...

:: 1. Check if tools are already in PATH
where cmake >nul 2>nul
if %ERRORLEVEL% EQU 0 goto build_cmake

where cl >nul 2>nul
if %ERRORLEVEL% EQU 0 goto build_msvc

where g++ >nul 2>nul
if %ERRORLEVEL% EQU 0 goto build_gpp

:: 2. Search for Visual Studio Installations (vswhere is best, but we'll try common paths)
echo [INFO] Tools not in PATH. Searching for Visual Studio...

set "VS_PATHS=C:\Program Files\Microsoft Visual Studio\2022\Community;C:\Program Files\Microsoft Visual Studio\2022\Enterprise;C:\Program Files\Microsoft Visual Studio\2022\Professional;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community;C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise;C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional"

for %%p in ("%VS_PATHS:;=" "%") do (
    if exist "%%~p\VC\Auxiliary\Build\vcvars64.bat" (
        echo [INFO] Found VS at: %%~p
        call "%%~p\VC\Auxiliary\Build\vcvars64.bat"
        goto build_msvc
    )
)

:: 3. Search for MinGW
echo [INFO] Searching for MinGW...
if exist "C:\msys64\mingw64\bin\g++.exe" (
    echo [INFO] Found MinGW at C:\msys64\mingw64\bin
    set "PATH=C:\msys64\mingw64\bin;%PATH%"
    goto build_gpp
)
if exist "C:\MinGW\bin\g++.exe" (
    echo [INFO] Found MinGW at C:\MinGW\bin
    set "PATH=C:\MinGW\bin;%PATH%"
    goto build_gpp
)

:: If we get here, nothing was found
goto no_compiler

:build_cmake
echo [INFO] CMake found.
cmake -S . -B build
if %ERRORLEVEL% NEQ 0 goto error
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 goto error
echo [SUCCESS] Build complete.
goto end

:build_msvc
echo [INFO] MSVC found.
if not exist build mkdir build
cl /std:c++17 /EHsc injector.cpp process_utils.cpp /Fe:build\injector.exe
if %ERRORLEVEL% NEQ 0 goto error
cl /LD test_dll.cpp user32.lib /Fe:build\test_dll.dll
if %ERRORLEVEL% NEQ 0 goto error
echo [SUCCESS] Build complete.
goto end

:build_gpp
echo [INFO] G++ found.
if not exist build mkdir build
g++ -o build/injector.exe injector.cpp process_utils.cpp -std=c++17
if %ERRORLEVEL% NEQ 0 goto error
g++ -shared -o build/test_dll.dll test_dll.cpp
if %ERRORLEVEL% NEQ 0 goto error
echo [SUCCESS] Build complete.
goto end

:no_compiler
echo [ERROR] No C++ compiler found.
echo.
echo Please install one of the following:
echo 1. Visual Studio 2022 Community (Free) - Select "Desktop development with C++"
echo 2. MinGW-w64
echo.
echo If you have installed them, try restarting your terminal or adding them to PATH.
goto error

:error
echo [FAIL] Build failed.
exit /b 1

:end
echo [DONE]
exit /b 0
