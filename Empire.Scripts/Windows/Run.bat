@echo off
rem Использование: Run.bat [Game^|Client^|Server^|Editor^|Tests] [Режим] [аргументы...]
setlocal
set "TARGET=%~1"
if "%TARGET%"=="" set "TARGET=Game"
set "MODE_IN=%~2"
call "%~dp0Common.bat" || exit /b 1
shift
shift
set "ARGS="
:collect
if "%~1"=="" goto run
set "ARGS=%ARGS% %1"
shift
goto collect
:run
if /i "%TARGET%"=="Tests" (
    "%BUILD_TOOL%" f -m test -y %EMPIRE_CONFIG_FLAGS%
    "%BUILD_TOOL%" build && "%BUILD_TOOL%" test
    exit /b %errorlevel%
)
"%BUILD_TOOL%" f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
"%BUILD_TOOL%" build Runtime.%TARGET%
if errorlevel 1 exit /b 1
"%BUILD_TOOL%" run Runtime.%TARGET%%ARGS%
exit /b %errorlevel%
