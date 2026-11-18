@echo off
rem Использование: RunSamples.bat [Режим]
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
set "EMPIRE_CONFIG_FLAGS=--samples=y %EMPIRE_CONFIG_FLAGS%"
xmake f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
xmake build
exit /b %errorlevel%
