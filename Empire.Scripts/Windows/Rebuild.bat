@echo off
rem Использование: Rebuild.bat [Режим] [Таргет]
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
xmake f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
xmake clean
if "%~2"=="" (xmake build) else (xmake build %~2)
exit /b %errorlevel%
