@echo off
rem Использование: Docs.bat  (заглушка)
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
echo [stub] Docs: Empire.Docs
exit /b %errorlevel%
