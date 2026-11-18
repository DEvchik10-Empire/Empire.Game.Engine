@echo off
rem Версия для разработки (с символами, редактор включён)
call "%~dp0Build.bat" Development
exit /b %errorlevel%
