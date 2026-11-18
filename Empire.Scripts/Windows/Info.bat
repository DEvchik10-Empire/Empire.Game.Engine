@echo off
rem Использование: Info.bat
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
echo Root: %ROOT%
"%BUILD_TOOL%" --version
ver
exit /b %errorlevel%
