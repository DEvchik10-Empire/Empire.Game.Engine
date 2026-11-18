@echo off
rem Использование: RunEditor.bat [Режим] [аргументы...]
setlocal
set "M=%~1"
if "%M%"=="" set "M=Development"
shift
set "A="
:c
if "%~1"=="" goto go
set "A=%A% %1"
shift
goto c
:go
call "%~dp0Run.bat" Editor %M%%A%
exit /b %errorlevel%
