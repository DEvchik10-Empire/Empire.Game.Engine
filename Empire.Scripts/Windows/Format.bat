@echo off
rem Использование: Format.bat  (заглушка)
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
where clang-format >nul 2>nul || (echo нет clang-format & exit /b 1)
for /r %%f in (*.hpp *.cpp) do echo %%f | findstr /i "ThirdParty" >nul || clang-format -i "%%f"
exit /b %errorlevel%
