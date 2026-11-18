@echo off
rem Использование: Clean.bat [All]
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
xmake clean
if /i "%~1"=="All" (
    rmdir /s /q "%ROOT%\Empire.Build" 2>nul
    rmdir /s /q "%ROOT%\.xmake" 2>nul
    rmdir /s /q "%ROOT%\build" 2>nul
)
exit /b %errorlevel%
