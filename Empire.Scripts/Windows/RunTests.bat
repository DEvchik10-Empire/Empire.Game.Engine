@echo off
rem Использование: RunTests.bat
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
"%BUILD_TOOL%" f -m test -y %EMPIRE_CONFIG_FLAGS%
"%BUILD_TOOL%" build && "%BUILD_TOOL%" test
exit /b %errorlevel%
