@echo off
rem Использование: CopyContent.bat [Режим]  — скопировать Docs, Content и README в папку сборки
setlocal
set "MODE_IN=%~1"
call "%~dp0Common.bat" || exit /b 1
set "MODE_NAME="
if /i "%MODE%"=="debug" set "MODE_NAME=Debug"
if /i "%MODE%"=="debuggame" set "MODE_NAME=DebugGame"
if /i "%MODE%"=="development" set "MODE_NAME=Development"
if /i "%MODE%"=="test" set "MODE_NAME=Test"
if /i "%MODE%"=="shipping" set "MODE_NAME=Shipping"
if "%MODE_NAME%"=="" (echo Неизвестный режим: %MODE% & exit /b 1)
set "OUT=%CD%\Empire.Build\Build.Windows.%MODE_NAME%"
if not exist "%OUT%" mkdir "%OUT%"
robocopy "Empire.Docs" "%OUT%\Build.Docs" /MIR /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 exit /b 1
robocopy "Empire.Content" "%OUT%\Build.Content" /MIR /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 exit /b 1
copy /Y "README.md" "%OUT%\README.md" >nul
echo Docs, Content и README скопированы в %OUT%
exit /b 0
