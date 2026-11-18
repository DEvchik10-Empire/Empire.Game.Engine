@echo off
rem Использование: RunServer.bat [Режим] [аргументы...]
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
call "%~dp0Run.bat" Server %M%%A%
exit /b %errorlevel%
