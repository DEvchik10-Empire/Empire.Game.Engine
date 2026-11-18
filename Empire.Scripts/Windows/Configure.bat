@echo off
rem Использование: Configure.bat [Режим]
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
xmake f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
exit /b %errorlevel%
