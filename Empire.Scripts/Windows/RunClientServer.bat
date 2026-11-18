@echo off
rem Использование: RunClientServer.bat [Режим]  — сервер в отдельном окне + клиент; при выходе клиента сервер останавливается
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
"%BUILD_TOOL%" f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
"%BUILD_TOOL%" build Runtime.Server Runtime.Client
if errorlevel 1 exit /b 1
start "Empire.Server" cmd /c "%BUILD_TOOL%" run Runtime.Server
"%BUILD_TOOL%" run Runtime.Client
set "RC=%errorlevel%"
taskkill /fi "WINDOWTITLE eq Empire.Server" /t /f >nul 2>nul
exit /b %RC%
