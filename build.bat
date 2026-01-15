@echo off
setlocal EnableDelayedExpansion

:: ===== Colors =====
set GREEN=[OK]
set RED=[ERROR]
set CYAN=[INFO]
set YELLOW=[TIME]

set OUT_DIR=build
set EXE_NAME=calculator.exe
set EXE_PATH=%OUT_DIR%\bin\%EXE_NAME%

cls

echo %CYAN% => Building project...

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
cd "%OUT_DIR%" || exit /b 1

cmake ..
if errorlevel 1 goto build_failed

cmake --build .
if errorlevel 1 goto build_failed

cd ..

if not exist "%EXE_PATH%" (
    echo %RED% ==> Executable not found: %EXE_PATH%
    exit /b 1
)

echo %GREEN% ==> Running...

set START_TIME=%TIME%

"%EXE_PATH%"

set END_TIME=%TIME%

call :calc_duration "%START_TIME%" "%END_TIME%"
echo.
echo %GREEN% => Finished in %YELLOW% %DURATION% ms
exit /b 0

:build_failed
echo %RED% ==> Build failed.
exit /b 1

:calc_duration
set "START=%~1"
set "END=%~2"

for /f "tokens=1-4 delims=:., " %%a in ("%START%") do (
    set /a ST=((%%a*3600 + %%b*60 + %%c)*1000 + %%d)
)
for /f "tokens=1-4 delims=:., " %%a in ("%END%") do (
    set /a ET=((%%a*3600 + %%b*60 + %%c)*1000 + %%d)
)

set /a DURATION=ET-ST
exit /b
