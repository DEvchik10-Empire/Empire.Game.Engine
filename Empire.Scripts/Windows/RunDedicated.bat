@echo off
rem Использование: RunDedicated.bat [порт] [Режим]
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
set "PORT=%~1"
if "%PORT%"=="" set "PORT=7777"
set "MODE=%~2"
if "%MODE%"=="" set "MODE=shipping"
set "EMPIRE_CONFIG_FLAGS=--editor=n %EMPIRE_CONFIG_FLAGS%"
xmake f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
xmake build Runtime.Server
xmake run Runtime.Server --port %PORT%
exit /b %errorlevel%
