@echo off
rem Использование: RunTests.bat
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
xmake f -m test -y %EMPIRE_CONFIG_FLAGS%
xmake build && xmake test
exit /b %errorlevel%
