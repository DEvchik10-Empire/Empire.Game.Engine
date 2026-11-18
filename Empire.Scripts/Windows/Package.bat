@echo off
rem Использование: Package.bat [Режим]  (заглушка)
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
if "%~1"=="" set "MODE=shipping"
xmake f -m %MODE% -y %EMPIRE_CONFIG_FLAGS%
if errorlevel 1 exit /b 1
xmake build Tools.Packager
echo [stub] Упаковка: подключить Tools.Packager
exit /b %errorlevel%
