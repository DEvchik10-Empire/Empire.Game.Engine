@echo off
rem Релизная версия (максимальная оптимизация, LTO, без редактора)
call "%~dp0Build.bat" Shipping
exit /b %errorlevel%
