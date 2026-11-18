@echo off
if not defined BUILD_TOOL set "BUILD_TOOL=build"
rem Общее: ROOT, переход в корень, MODE (нижний регистр) из MODE_IN (по умолчанию Development).
rem Подключение: set "MODE_IN=%~1" & call "%~dp0Common.bat"
set "ROOT=%~dp0..\.."
cd /d "%ROOT%"
set "MODE=%MODE_IN%"
if "%MODE%"=="" set "MODE=Development"
for %%a in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do call set "MODE=%%MODE:%%a=%%a%%"
exit /b 0
