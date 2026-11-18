@echo off
rem Использование: RunClientServer.bat [Режим]
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
xmake f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
xmake build Runtime.Server Runtime.Client
start "Empire.Server" xmake run Runtime.Server
xmake run Runtime.Client
exit /b %errorlevel%
