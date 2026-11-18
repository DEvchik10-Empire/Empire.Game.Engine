@echo off
rem Использование: Build.bat [Режим] [Таргет]   Режимы: Debug^|DebugGame^|Development^|Test^|Shipping
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
"%BUILD_TOOL%" f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
if "%~2"=="" ("%BUILD_TOOL%" build) else ("%BUILD_TOOL%" build %~2)
if errorlevel 1 exit /b 1
call "%~dp0CopyContent.bat" %MODE%
exit /b %errorlevel%
