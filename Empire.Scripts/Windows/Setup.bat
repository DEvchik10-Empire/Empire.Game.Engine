@echo off
rem Ставит xmake (заглушка: winget / scoop)
where xmake >nul 2>nul && (echo xmake уже установлен & exit /b 0)
winget install xmake-io.xmake || scoop install xmake
exit /b %errorlevel%
