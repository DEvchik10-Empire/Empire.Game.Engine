@echo off
rem Использование: CompileAssets.bat  (заглушка)
setlocal
set "MODE_IN="
call "%~dp0Common.bat" || exit /b 1
xmake build Tools.AssetCompiler Tools.ShaderCompiler
echo [stub] Компиляция ассетов/шейдеров
exit /b %errorlevel%
