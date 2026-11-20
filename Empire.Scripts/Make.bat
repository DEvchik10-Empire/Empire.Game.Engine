@echo off
setlocal EnableExtensions DisableDelayedExpansion

for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "TOOLS_MAKE=%ROOT%\Empire.Runtime\Runtime.Tools\Tools.Make"
set "PROJECT_FILE=%ROOT%\make.lua"
if not exist "%PROJECT_FILE%" (
    echo Make: не знайдено make.lua в "%ROOT%".
    exit /b 2
)
if not exist "%TOOLS_MAKE%\configure" (
    echo Make: не знайдено вихідні файли MakeTool.
    exit /b 2
)

set "COMMAND=%~1"
if not defined COMMAND goto no_arguments
shift
set "MODE=development"
set "MODE_LABEL=Development"
set "MODE_EXPLICIT="
set "TARGET_PLATFORM="
set "PRODUCT="
set "PLATFORM_LABEL=Windows"
set "JOBS=%NUMBER_OF_PROCESSORS%"
if not defined JOBS set "JOBS=2"
set "VERBOSE="
set "QUIET="
set "FORCE="
set "CLEAN_ALL="
set "EDITOR_OPTION="
set "TESTS_OPTION="
set "SAMPLES_OPTION="
set "PORT=7777"
set "PORT_EXPLICIT="
set "POSITIONAL="
set "APP_ARGS="
set "AFTER_DASH="

if /i "%COMMAND%"=="-bs" set "COMMAND=Bootstrap"
if /i "%COMMAND%"=="-cf" set "COMMAND=Configure"
if /i "%COMMAND%"=="-b" set "COMMAND=Build"
if /i "%COMMAND%"=="-rb" set "COMMAND=Rebuild"
if /i "%COMMAND%"=="-c" set "COMMAND=Clean"
if /i "%COMMAND%"=="-cc" set "COMMAND=CopyContent"
if /i "%COMMAND%"=="-i" set "COMMAND=Info"
if /i "%COMMAND%"=="-r" set "COMMAND=Run"
if /i "%COMMAND%"=="-re" set "COMMAND=RunEditor"
if /i "%COMMAND%"=="-rc" set "COMMAND=RunClient"
if /i "%COMMAND%"=="-rs" set "COMMAND=RunServer"
if /i "%COMMAND%"=="-rd" set "COMMAND=RunDedicated"
if /i "%COMMAND%"=="-rcs" set "COMMAND=RunClientServer"
if /i "%COMMAND%"=="-t" set "COMMAND=RunTests"
goto parse

:no_arguments
echo Empire.Game.Engine - система збирання Make
echo Повна довідка: "%~f0" Help
echo.
echo Приклади збірки:
echo   Make.bat Bootstrap Development
echo   Make.bat Build Engine Development -j8
echo   Make.bat Build Game DebugGame -j8
echo   Make.bat Build Game Shipping -j8
exit /b 0

:parse
if "%~1"=="" goto parsed
if "%AFTER_DASH%"=="1" (
    set "APP_ARGS=%APP_ARGS% %1"
    shift
    goto parse
)
if "%~1"=="--" (
    set "AFTER_DASH=1"
    shift
    goto parse
)
if /i "%~1"=="-Debug" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=debug"&set "MODE_LABEL=Debug"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="-DebugGame" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=debuggame"&set "MODE_LABEL=DebugGame"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="-Development" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=development"&set "MODE_LABEL=Development"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="-Test" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=test"&set "MODE_LABEL=Test"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="-Shipping" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=shipping"&set "MODE_LABEL=Shipping"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="-Release" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=shipping"&set "MODE_LABEL=Shipping"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="-Engine" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=Engine"&shift&goto parse)
if /i "%~1"=="-Game" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=Game"&shift&goto parse)
if /i "%~1"=="-MakeTool" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=MakeTool"&shift&goto parse)
if /i "%~1"=="-Linux" (if defined TARGET_PLATFORM (echo Make: вкажіть лише одну цільову платформу.&exit /b 2)&set "TARGET_PLATFORM=linux"&set "PLATFORM_LABEL=Linux"&shift&goto parse)
if /i "%~1"=="-Windows" (if defined TARGET_PLATFORM (echo Make: вкажіть лише одну цільову платформу.&exit /b 2)&set "TARGET_PLATFORM=windows"&set "PLATFORM_LABEL=Windows"&shift&goto parse)
if /i "%~1"=="-MacOS" (if defined TARGET_PLATFORM (echo Make: вкажіть лише одну цільову платформу.&exit /b 2)&set "TARGET_PLATFORM=macosx"&set "PLATFORM_LABEL=MacOS"&shift&goto parse)
if /i "%~1"=="-Android" (if defined TARGET_PLATFORM (echo Make: вкажіть лише одну цільову платформу.&exit /b 2)&set "TARGET_PLATFORM=android"&set "PLATFORM_LABEL=Android"&shift&goto parse)
if /i "%~1"=="-iOS" (if defined TARGET_PLATFORM (echo Make: вкажіть лише одну цільову платформу.&exit /b 2)&set "TARGET_PLATFORM=iphoneos"&set "PLATFORM_LABEL=iOS"&shift&goto parse)
if /i "%~1"=="-WASM" (if defined TARGET_PLATFORM (echo Make: вкажіть лише одну цільову платформу.&exit /b 2)&set "TARGET_PLATFORM=wasm"&set "PLATFORM_LABEL=WASM"&shift&goto parse)
if /i "%~1"=="-Verbose" (set "VERBOSE=-v"&shift&goto parse)
if /i "%~1"=="-Quiet" (set "QUIET=-q"&shift&goto parse)
if /i "%~1"=="-Force" (set "FORCE=-r"&shift&goto parse)
if /i "%~1"=="-All" (set "CLEAN_ALL=1"&shift&goto parse)
if /i "%~1"=="-Editor" (if defined EDITOR_OPTION (echo Make: вкажіть лише -Editor або -NoEditor.&exit /b 2)&set "EDITOR_OPTION=--editor=y"&shift&goto parse)
if /i "%~1"=="-NoEditor" (if defined EDITOR_OPTION (echo Make: вкажіть лише -Editor або -NoEditor.&exit /b 2)&set "EDITOR_OPTION=--editor=n"&shift&goto parse)
if /i "%~1"=="-Tests" (set "TESTS_OPTION=--tests=y"&shift&goto parse)
if /i "%~1"=="-Samples" (set "SAMPLES_OPTION=--samples=y"&shift&goto parse)
if /i "%~1"=="-Port" (
    set "PORT=%~2"
    set "PORT_EXPLICIT=1"
    shift
    shift
    goto parse
)
set "ARG=%~1"
if /i "%ARG:~0,5%"=="-Port" (
    for /f "tokens=2 delims==" %%P in ("%ARG%") do set "PORT=%%P"
    set "PORT_EXPLICIT=1"
    shift
    goto parse
)
if "%~1"=="-j" (
    if "%~2"=="" (echo Make: параметр -j потребує кількість задач.&exit /b 2)
    set "JOBS=%~2"
    shift
    shift
    goto parse
)
if "%ARG:~0,2%"=="-j" (
    set "JOBS=%ARG:~2%"
    shift
    goto parse
)
if "%ARG:~0,1%"=="-" (
    echo Make: невідомий параметр "%~1". Використайте -Help.
    exit /b 2
)
if /i "%COMMAND%"=="Build" if /i "%~1"=="Engine" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=Engine"&shift&goto parse)
if /i "%COMMAND%"=="Build" if /i "%~1"=="Game" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=Game"&shift&goto parse)
if /i "%COMMAND%"=="Build" if /i "%~1"=="MakeTool" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=MakeTool"&shift&goto parse)
if /i "%COMMAND%"=="Rebuild" if /i "%~1"=="Engine" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=Engine"&shift&goto parse)
if /i "%COMMAND%"=="Rebuild" if /i "%~1"=="Game" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=Game"&shift&goto parse)
if /i "%COMMAND%"=="Rebuild" if /i "%~1"=="MakeTool" (if defined PRODUCT (echo Make: вкажіть лише один продукт.&exit /b 2)&set "PRODUCT=MakeTool"&shift&goto parse)
if /i "%~1"=="Debug" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=debug"&set "MODE_LABEL=Debug"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="Development" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=development"&set "MODE_LABEL=Development"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="Test" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=test"&set "MODE_LABEL=Test"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="Shipping" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=shipping"&set "MODE_LABEL=Shipping"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="Release" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=shipping"&set "MODE_LABEL=Shipping"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="DebugGame" (if defined MODE_EXPLICIT (echo Make: вкажіть лише один режим.&exit /b 2)&set "MODE=debuggame"&set "MODE_LABEL=DebugGame"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%~1"=="Linux" (set "TARGET_PLATFORM=linux"&set "PLATFORM_LABEL=Linux"&shift&goto parse)
if /i "%~1"=="Windows" (set "TARGET_PLATFORM=windows"&set "PLATFORM_LABEL=Windows"&shift&goto parse)
if /i "%~1"=="MacOS" (set "TARGET_PLATFORM=macosx"&set "PLATFORM_LABEL=MacOS"&shift&goto parse)
if /i "%~1"=="Android" (set "TARGET_PLATFORM=android"&set "PLATFORM_LABEL=Android"&shift&goto parse)
if /i "%~1"=="iOS" (set "TARGET_PLATFORM=iphoneos"&set "PLATFORM_LABEL=iOS"&shift&goto parse)
if /i "%~1"=="WASM" (set "TARGET_PLATFORM=wasm"&set "PLATFORM_LABEL=WASM"&shift&goto parse)
if /i "%COMMAND%"=="Bootstrap" if /i "%~1"=="Debug" (set "MODE=debug"&set "MODE_LABEL=Debug"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%COMMAND%"=="Bootstrap" if /i "%~1"=="Development" (set "MODE=development"&set "MODE_LABEL=Development"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%COMMAND%"=="Bootstrap" if /i "%~1"=="Test" (set "MODE=test"&set "MODE_LABEL=Test"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%COMMAND%"=="Bootstrap" if /i "%~1"=="Shipping" (set "MODE=shipping"&set "MODE_LABEL=Shipping"&set "MODE_EXPLICIT=1"&shift&goto parse)
if /i "%COMMAND%"=="Bootstrap" if /i "%~1"=="Release" (set "MODE=shipping"&set "MODE_LABEL=Shipping"&set "MODE_EXPLICIT=1"&shift&goto parse)
set "POSITIONAL=%POSITIONAL% %1"
shift
goto parse

:parsed
if /i "%COMMAND%"=="Help" goto help
if /i "%COMMAND%"=="-Help" goto help
if /i "%COMMAND%"=="--help" goto help
if /i "%COMMAND%"=="-h" goto help
if /i "%COMMAND%"=="Version" goto version
if /i "%COMMAND%"=="-Version" goto version
if /i not "%JOBS%"=="" (
    for /f "delims=0123456789" %%A in ("%JOBS%") do if not "%%A"=="" (echo Make: неправильна кількість задач.&exit /b 2)
    if "%JOBS%"=="0" (echo Make: кількість задач має бути більшою за нуль.&exit /b 2)
)
if not defined EMPIRE_CONFIG_FLAGS set "EMPIRE_CONFIG_FLAGS="
set "CONFIG_FLAGS=%EDITOR_OPTION% %TESTS_OPTION% %SAMPLES_OPTION% %EMPIRE_CONFIG_FLAGS%"
if /i "%MODE%"=="shipping" (
    if /i "%EDITOR_OPTION%"=="--editor=y" (echo Make: -Editor несумісний із Shipping; редактор вимкнений.&exit /b 2)
    if not defined EDITOR_OPTION set "CONFIG_FLAGS=--editor=n %TESTS_OPTION% %SAMPLES_OPTION% %EMPIRE_CONFIG_FLAGS%"
)
if /i "%MODE%"=="test" if not defined TESTS_OPTION set "CONFIG_FLAGS=%EDITOR_OPTION% --tests=y %SAMPLES_OPTION% %EMPIRE_CONFIG_FLAGS%"
if defined CLEAN_ALL if /i not "%COMMAND%"=="Clean" (echo Make: -All підтримується лише командою Clean.&exit /b 2)
if defined PORT_EXPLICIT if /i not "%COMMAND%"=="RunDedicated" (echo Make: -Port підтримується лише RunDedicated.&exit /b 2)
if defined FORCE if /i not "%COMMAND%"=="Build" if /i not "%COMMAND%"=="Rebuild" if /i not "%COMMAND%"=="Bootstrap" (echo Make: -Force підтримується лише Build, Rebuild та Bootstrap.&exit /b 2)

if /i "%COMMAND%"=="Bootstrap" (
    if defined PRODUCT (echo Make: Bootstrap збирає лише MakeTool; не вказуйте селектори продукту.&exit /b 2)
    if defined TARGET_PLATFORM (echo Make: Bootstrap збирає MakeTool лише для ОС збірочної машини.&exit /b 2)
    if defined POSITIONAL (echo Make: Bootstrap не приймає назви цілей проєкту.&exit /b 2)
    if defined EDITOR_OPTION (echo Make: параметри Editor/Tests/Samples не застосовуються до Bootstrap.&exit /b 2)
    if defined TESTS_OPTION (echo Make: параметри Editor/Tests/Samples не застосовуються до Bootstrap.&exit /b 2)
    if defined SAMPLES_OPTION (echo Make: параметри Editor/Tests/Samples не застосовуються до Bootstrap.&exit /b 2)
    if defined VERBOSE (echo Make: -Verbose/-Quiet не застосовуються до bootstrap MakeTool.&exit /b 2)
    if defined QUIET (echo Make: -Verbose/-Quiet не застосовуються до bootstrap MakeTool.&exit /b 2)
    if not defined MODE_EXPLICIT (echo Make: для Bootstrap обов’язково вкажіть режим: -Debug, -Development, -Test або -Shipping.&exit /b 2)
    if /i "%MODE_LABEL%"=="DebugGame" (echo Make: DebugGame доступний лише для Game, а не для MakeTool.&exit /b 2)
    goto bootstrap
)
if /i "%COMMAND%"=="CopyContent" goto copy_content_command
if /i "%COMMAND%"=="RunTests" (
    if defined MODE_EXPLICIT if /i not "%MODE%"=="test" (echo Make: RunTests завжди використовує режим Test.&exit /b 2)
    set "MODE=test"
    set "MODE_LABEL=Test"
    set "CONFIG_FLAGS=%CONFIG_FLAGS% --tests=y"
)
if /i "%COMMAND%"=="Build" if not defined MODE_EXPLICIT (echo Make: для Build обов’язково вкажіть режим збірки.&exit /b 2)
if /i "%COMMAND%"=="Rebuild" (
    if not defined MODE_EXPLICIT (echo Make: для Rebuild обов’язково вкажіть режим збірки.&exit /b 2)
    if not defined PRODUCT (echo Make: для Rebuild вкажіть -Engine, -Game або -MakeTool.&exit /b 2)
    if /i "%PRODUCT%"=="MakeTool" (
        if defined TARGET_PLATFORM (echo Make: MakeTool збирається лише для ОС збірочної машини.&exit /b 2)
        if /i "%MODE_LABEL%"=="DebugGame" (echo Make: DebugGame доступний лише для Game.&exit /b 2)
        if defined POSITIONAL (echo Make: для -MakeTool не можна вказувати цілі проєкту.&exit /b 2)
        if defined EDITOR_OPTION (echo Make: -Editor/-NoEditor не застосовуються до MakeTool.&exit /b 2)
        if defined TESTS_OPTION (echo Make: -Tests не застосовується до MakeTool.&exit /b 2)
        if defined SAMPLES_OPTION (echo Make: -Samples не застосовується до MakeTool.&exit /b 2)
        if defined VERBOSE (echo Make: -Verbose/-Quiet не застосовуються до bootstrap MakeTool.&exit /b 2)
        if defined QUIET (echo Make: -Verbose/-Quiet не застосовуються до bootstrap MakeTool.&exit /b 2)
        set "FORCE=1"
        goto bootstrap
    )
    if /i "%PRODUCT%"=="Engine" if /i "%MODE_LABEL%"=="DebugGame" (echo Make: DebugGame доступний лише для Game.&exit /b 2)
)
if /i "%COMMAND%"=="Build" (
    if not defined PRODUCT (echo Make: для Build вкажіть -Engine, -Game або -MakeTool.&exit /b 2)
    if /i "%PRODUCT%"=="MakeTool" (
        if defined TARGET_PLATFORM (echo Make: MakeTool збирається лише для ОС збірочної машини.&exit /b 2)
        if /i "%MODE_LABEL%"=="DebugGame" (echo Make: DebugGame доступний лише для Game.&exit /b 2)
        if defined POSITIONAL (echo Make: для -MakeTool не можна вказувати цілі проєкту.&exit /b 2)
        if defined EDITOR_OPTION (echo Make: -Editor/-NoEditor не застосовуються до MakeTool.&exit /b 2)
        if defined TESTS_OPTION (echo Make: -Tests не застосовується до MakeTool.&exit /b 2)
        if defined SAMPLES_OPTION (echo Make: -Samples не застосовується до MakeTool.&exit /b 2)
        if defined VERBOSE (echo Make: -Verbose/-Quiet не застосовуються до bootstrap MakeTool.&exit /b 2)
        if defined QUIET (echo Make: -Verbose/-Quiet не застосовуються до bootstrap MakeTool.&exit /b 2)
        goto bootstrap
    )
    if /i "%PRODUCT%"=="Engine" if /i "%MODE_LABEL%"=="DebugGame" (echo Make: DebugGame доступний лише для Game.&exit /b 2)
)
if /i not "%COMMAND%"=="Build" if /i not "%COMMAND%"=="Rebuild" if defined PRODUCT (
    echo Make: -Engine, -Game та -MakeTool використовуються лише з Build або Rebuild.
    exit /b 2
)
call :FindTool
if not defined BUILD_TOOL (
    echo Make: не знайдено жодної збірки MakeTool для Windows.
    echo Спочатку виконайте: Make.bat Bootstrap Development
    exit /b 1
)
for %%I in ("%BUILD_TOOL%") do set "MAKETOOL_PROGRAM_DIR=%%~dpI"
set "MAKETOOL_PROGRAM_DIR=%MAKETOOL_PROGRAM_DIR%Make.Runtime"
if not exist "%MAKETOOL_PROGRAM_DIR%" set "MAKETOOL_PROGRAM_DIR=%TOOLS_MAKE%\Make.Runtime"
set "XMAKE_PROGRAM_DIR=%MAKETOOL_PROGRAM_DIR%"

if /i "%COMMAND%"=="Configure" (
    call :ConfigureProject || exit /b 1
    exit /b 0
)
if /i "%COMMAND%"=="Build" (
    call :ConfigureProject || exit /b 1
    call:BuildProject || exit /b 1
    if /i "%PRODUCT%"=="Game" call :CopyContent || exit /b 1
    exit /b 0
)
if /i "%COMMAND%"=="Rebuild" (
    call :ConfigureProject || exit /b 1
    set "FORCE=1"
    call :BuildProject || exit /b 1
    if /i "%PRODUCT%"=="Game" call :CopyContent || exit /b 1
    exit /b 0
)
if /i "%COMMAND%"=="Clean" (
    call :RunTool clean -a || exit /b 1
    if defined CLEAN_ALL (
        if exist "%ROOT%\Empire.Build" rmdir /s /q "%ROOT%\Empire.Build"
        if exist "%ROOT%\.MakeTool" rmdir /s /q "%ROOT%\.MakeTool"
        if exist "%ROOT%\build" rmdir /s /q "%ROOT%\build"
    )
    exit /b 0
)
if /i "%COMMAND%"=="Info" goto info
if /i "%COMMAND%"=="RunTests" (
    set "MODE=test"
    set "MODE_LABEL=Test"
    call :ConfigureProject || exit /b 1
    call :RunTool build -j "%JOBS%" || exit /b 1
    call :RunTool test -j "%JOBS%" || exit /b 1
    exit /b 0
)
if /i "%COMMAND%"=="RunDedicated" (
    if not defined MODE_EXPLICIT (set "MODE=shipping"&set "MODE_LABEL=Shipping")
    set "CONFIG_FLAGS=%CONFIG_FLAGS% --editor=n"
    call :ConfigureProject || exit /b 1
    call :RunTool build -j "%JOBS%" Runtime.Server || exit /b 1
    call :RunTool run Runtime.Server --port "%PORT%" %APP_ARGS% || exit /b 1
    exit /b 0
)
if /i "%COMMAND%"=="RunClientServer" (
    call :ConfigureProject || exit /b 1
    call :RunTool build -j "%JOBS%" Runtime.Server Runtime.Client || exit /b 1
    start "Empire Server" "%BUILD_TOOL%" run -P "%ROOT%" -F "%PROJECT_FILE%" Runtime.Server
    call :RunTool run Runtime.Client %APP_ARGS% || exit /b 1
    exit /b 0
)
if /i "%COMMAND%"=="Run" goto run_target
if /i "%COMMAND%"=="RunGame" (set "POSITIONAL= Game"&goto run_target)
if /i "%COMMAND%"=="RunEditor" (set "POSITIONAL= Editor"&goto run_target)
if /i "%COMMAND%"=="RunClient" (set "POSITIONAL= Client"&goto run_target)
if /i "%COMMAND%"=="RunServer" (set "POSITIONAL= Server"&goto run_target)

echo Make: невідома команда "%COMMAND%". Використайте -Help.
exit /b 2

:run_target
for %%T in (%POSITIONAL%) do if not defined RUN_TARGET set "RUN_TARGET=%%~T"
if not defined RUN_TARGET set "RUN_TARGET=Game"
if /i "%RUN_TARGET%"=="Tests" goto runtests
if /i not "%RUN_TARGET%"=="Game" if /i not "%RUN_TARGET%"=="Client" if /i not "%RUN_TARGET%"=="Server" if /i not "%RUN_TARGET%"=="Editor" (echo Make: невідома ціль Run "%RUN_TARGET%".&exit /b 2)
call :ConfigureProject || exit /b 1
call :RunTool build -j "%JOBS%" "Runtime.%RUN_TARGET%" || exit /b 1
call :RunTool run "Runtime.%RUN_TARGET%" %APP_ARGS%
exit /b %errorlevel%

:runtests
set "MODE=test"
set "MODE_LABEL=Test"
call :ConfigureProject || exit /b 1
call :RunTool build -j "%JOBS%" || exit /b 1
call :RunTool test -j "%JOBS%"
exit /b %errorlevel%

:bootstrap
call :BootstrapTool
exit /b %errorlevel%

:copy_content_command
call :CopyContent || exit /b 1
exit /b 0

:FindTool
set "BUILD_TOOL="
if defined EMPIRE_BUILD_TOOL if exist "%EMPIRE_BUILD_TOOL%" set "BUILD_TOOL=%EMPIRE_BUILD_TOOL%"
if defined BUILD_TOOL exit /b 0
if exist "%ROOT%\Empire.Build\Build.Windows.MakeTool.Development\" for /r "%ROOT%\Empire.Build\Build.Windows.MakeTool.Development" %%F in (MakeTool.exe) do if not defined BUILD_TOOL set "BUILD_TOOL=%%~fF"
if defined BUILD_TOOL exit /b 0
for %%D in ("%ROOT%\Empire.Build\Build.Windows.MakeTool.*") do if exist "%%~fD\" for /r "%%~fD" %%F in (MakeTool.exe) do if not defined BUILD_TOOL set "BUILD_TOOL=%%~fF"
exit /b 0

:BootstrapTool
where sh.exe >nul 2>nul || where sh >nul 2>nul || (echo Make: потрібен sh. Встановіть MSYS2 або Git for Windows.&exit /b 1)
where make.exe >nul 2>nul || where make >nul 2>nul || (echo Make: потрібен GNU Make.&exit /b 1)
if not exist "%TOOLS_MAKE%\Make.Kernel\TBOX\tbox\src\xmake.sh" (
    echo Make: неповні вихідні файли MakeTool (відсутній TBOX).
    echo У цьому архіві відсутні деякі залежності. Спочатку отримайте повний вихідний код з усіма вкладеними залежностями.
    exit /b 1
)
if not exist "%TOOLS_MAKE%\Make.Kernel\Lua\lua\lapi.c" (
    echo Make: неповні вихідні файли MakeTool (відсутній Lua).
    exit /b 1
)
if not exist "%TOOLS_MAKE%\Make.Kernel\LZ4\lz4\lib\lz4.c" (
    echo Make: неповні вихідні файли MakeTool (відсутній LZ4).
    exit /b 1
)
if not exist "%TOOLS_MAKE%\Make.Kernel\CJSON\lua-cjson\lua_cjson.c" (
    echo Make: неповні вихідні файли MakeTool (відсутній Lua-CJSON).
    exit /b 1
)
if not exist "%TOOLS_MAKE%\Make.Kernel\xxHash\xxhash\xxhash.h" (
    echo Make: неповні вихідні файли MakeTool (відсутній xxHash).
    exit /b 1
)
dir /b "%TOOLS_MAKE%\Make.Kernel\SV\sv\src\*.c" >nul 2>nul || (echo Make: неповні вихідні файли MakeTool (відсутній SV).&exit /b 1)
echo Збирання MakeTool з вихідного коду: %TOOLS_MAKE%
set "TOOL_MODE_LABEL=%MODE_LABEL%"
if /i "%TOOL_MODE_LABEL%"=="DebugGame" set "TOOL_MODE_LABEL=Development"
set "TOOL_BUILD_MODE=%MODE%"
if /i "%TOOL_BUILD_MODE%"=="debuggame" set "TOOL_BUILD_MODE=development"
if not exist "%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%" mkdir "%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%"
pushd "%TOOLS_MAKE%" || exit /b 1
sh ./configure --generator=gmake --plat=mingw --mode=%TOOL_BUILD_MODE% --builddir=../../../Empire.Build/Build.Windows.MakeTool.%TOOL_MODE_LABEL%
if errorlevel 1 (popd&echo Make: не вдалося налаштувати MakeTool.&exit /b 1)
if defined FORCE (make -B -j%JOBS%) else (make -j%JOBS%)
if errorlevel 1 (popd&echo Make: не вдалося зібрати MakeTool.&exit /b 1)
popd
if not exist "%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%\MakeTool.exe" (echo Make: виконуваний файл MakeTool.exe не створено.&exit /b 1)
if exist "%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%\Make.Runtime" rmdir /s /q "%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%\Make.Runtime"
xcopy "%TOOLS_MAKE%\Make.Runtime" "%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%\Make.Runtime\" /E /I /Q >nul
for /f "tokens=2 delims==" %%V in ('findstr /b "MakeToolVersion" "%PROJECT_FILE%"') do >"%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%\Make.Runtime\Make.Version" echo %%~V
set "BUILD_TOOL=%ROOT%\Empire.Build\Build.Windows.MakeTool.%TOOL_MODE_LABEL%\MakeTool.exe"
if not exist "%BUILD_TOOL%" (echo Make: збирання MakeTool завершилося, але виконуваний файл не знайдено в потрібному каталозі.&exit /b 1)
echo MakeTool готовий: %BUILD_TOOL%
exit /b 0

:ConfigureProject
if defined TARGET_PLATFORM (call :RunTool f -m %MODE% -p %TARGET_PLATFORM% -y %CONFIG_FLAGS%) else call :RunTool f -m %MODE% -y %CONFIG_FLAGS%
exit /b %errorlevel%

:BuildProject
set "GROUP_ARGS="
if defined PRODUCT set "GROUP_ARGS=-g %PRODUCT%"
if defined FORCE (call :RunTool build -r -j "%JOBS%" %GROUP_ARGS% %POSITIONAL%) else call :RunTool build -j "%JOBS%" %GROUP_ARGS% %POSITIONAL%
exit /b %errorlevel%

:RunTool
set "ACTION=%~1"
shift
set "TOOL_ARGS="
:RunToolCollect
if "%~1"=="" goto RunToolExecute
set TOOL_ARGS=%TOOL_ARGS% "%~1"
shift
goto RunToolCollect
:RunToolExecute
"%BUILD_TOOL%" %ACTION% %VERBOSE% %QUIET% -P "%ROOT%" -F "%PROJECT_FILE%" %TOOL_ARGS%
exit /b %errorlevel%

:CopyContent
set "OUT=%ROOT%\Empire.Build\Build.%PLATFORM_LABEL%.Game.%MODE_LABEL%"
if not exist "%OUT%" mkdir "%OUT%"
if exist "%ROOT%\Empire.Docs" robocopy "%ROOT%\Empire.Docs" "%OUT%\Build.Docs" /MIR /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 exit /b 1
if exist "%ROOT%\Empire.Content" robocopy "%ROOT%\Empire.Content" "%OUT%\Build.Content" /MIR /NFL /NDL /NJH /NJS /NP >nul
if errorlevel 8 exit /b 1
if exist "%ROOT%\README.md" copy /Y "%ROOT%\README.md" "%OUT%\README.md" >nul
echo Ресурси скопійовано до %OUT%
exit /b 0

:version
for /f "tokens=2 delims==" %%V in ('findstr /b "EngineVersion" "%PROJECT_FILE%"') do set "ENGINE_VERSION=%%~V"
if not defined ENGINE_VERSION set "ENGINE_VERSION=не задана в make.lua"
echo Версія Empire.Game.Engine: %ENGINE_VERSION%
set "MAKETOOL_VERSION="
for /f "tokens=2 delims==" %%V in ('findstr /b "MakeToolVersion" "%PROJECT_FILE%"') do set "MAKETOOL_VERSION=%%~V"
if not defined MAKETOOL_VERSION set "MAKETOOL_VERSION=не задана в make.lua"
echo Версія MakeTool: %MAKETOOL_VERSION%
exit /b 0

:info
echo Відомості про систему та проєкт:
echo   Проєкт: %ROOT%
echo   Файл конфігурації: %PROJECT_FILE%
echo   MakeTool: %BUILD_TOOL%
powershell -NoProfile -ExecutionPolicy Bypass -Command "$os=Get-CimInstance Win32_OperatingSystem; $cpu=Get-CimInstance Win32_Processor | Select-Object -First 1; $gpu=Get-CimInstance Win32_VideoController | Select-Object -First 1; $ram=[math]::Round($os.TotalVisibleMemorySize/1MB,1); Write-Output ('  Операційна система: ' + $os.Caption + ' ' + $os.Version); Write-Output ('  Архітектура: ' + $env:PROCESSOR_ARCHITECTURE); Write-Output ('  Процесор: ' + $cpu.Name + ' (' + $cpu.NumberOfLogicalProcessors + ' потоків)'); Write-Output ('  Оперативна пам’ять: ' + $ram + ' ГБ'); if($gpu){Write-Output ('  Відеокарта: ' + $gpu.Name)}; $drive=Get-PSDrive -Name ($env:SystemDrive.TrimEnd(':')); if($drive){Write-Output ('  Вільне місце на диску: ' + [math]::Round($drive.Free/1GB,1) + ' ГБ')}"
exit /b 0

:help
echo Empire.Game.Engine - Make
echo.
echo ШВИДКИЙ СТАРТ
echo   1. Make.bat Bootstrap Development
echo   2. Make.bat Build Engine Development -j8
echo   3. Make.bat Build Game DebugGame -j8
echo   Повна довідка та інші приклади: Make.bat Help
echo.
echo КОМАНДИ
echo   Build, Rebuild   Зібрати або перебудувати один продукт; потрібні продукт і режим
echo   Bootstrap        Зібрати MakeTool з вихідного коду; режим обов’язковий
echo   Configure        Налаштувати проєкт без компіляції
echo   Clean            Очистити артефакти; -All видаляє всі build-каталоги й кеші
echo   Run, RunEditor, RunClient, RunServer, RunDedicated, RunClientServer
echo   RunTests         Зібрати й запустити тести
echo   CopyContent      Скопіювати ресурси до Game
echo   Info, Version, Help
echo.
echo ПРОДУКТИ ТА РЕЖИМИ
echo   Engine:   Debug Development Test Shipping
echo   Game:     Debug DebugGame Development Test Shipping
echo   MakeTool: Debug Development Test Shipping
echo   Для Build/Rebuild вкажіть продукт і режим без дефісів: Build Engine Shipping
echo   Також обов'язково вкажіть рівно один режим.
echo   DebugGame доступний лише для Game; Engine у цьому режимі використовує Development.
echo   Release є псевдонімом Shipping.
echo.
echo MAKETOOL
echo   Збирається окремо для ОС, на якій запускається Make. Не вказуйте платформу.
echo   Engine/Game використовують будь-яку наявну збірку MakeTool для host-ОС.
echo   Автоматичної збірки MakeTool немає. Спочатку виконайте:
echo     Make.bat Bootstrap Development
echo.
echo ПЛАТФОРМИ
echo   Linux Windows MacOS Android iOS WASM
echo   Платформа задає ціль Engine/Game; MakeTool завжди збирається для host-ОС.
echo.
echo ПОШИРЕНІ ПАРАМЕТРИ
echo   -j8 або -j 8   Кількість паралельних задач
echo   -Verbose       Докладний вивід; -Quiet - скорочений
echo   Для MakeTool ці два параметри не підтримуються
echo   -Force         Примусово перебудувати ціль
echo   -Editor        Увімкнути редактор; у Shipping він вимкнений
echo   -NoEditor      Вимкнути редактор
echo   -Tests         Додати тестові цілі поза Test
echo   -Samples       Додати Empire.Samples
echo   -Port=7777     Порт RunDedicated
echo   -All           З Clean видалити Empire.Build, .MakeTool та build
echo   --              Наступні аргументи передати програмі
echo.
echo ПРИКЛАДИ
echo   Make.bat Build MakeTool Debug
echo   Make.bat Bootstrap Development
echo   Make.bat Build MakeTool Test
echo   Make.bat Build MakeTool Shipping
echo   Make.bat Build Engine Development -j8
echo   Make.bat Build Engine Shipping -j8
echo   Make.bat Build Game DebugGame -j8
echo   Make.bat Build Game Shipping -j8
echo   Make.bat Rebuild Windows Engine Shipping -j8
echo   Make.bat RunEditor -Development
echo   Make.bat RunTests
echo   Make.bat RunDedicated -Shipping -Port=7777
echo   Make.bat Clean
echo   Make.bat Clean -All
echo   Make.bat Info
echo   Make.bat Version
echo.
echo КАТАЛОГИ
echo   Empire.Build\Build.Windows.Engine.Shipping\
echo   Empire.Build\Build.Windows.Game.DebugGame\
echo   Empire.Build\Build.Windows.MakeTool.Development\
echo.
echo ВЕРСІЇ
echo   EngineVersion та MakeToolVersion задаються у кореневому make.lua.
echo   set_version() - внутрішня версія Xmake.
exit /b 0
