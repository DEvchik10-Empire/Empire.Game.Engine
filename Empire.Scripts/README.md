# Empire.Scripts

Скрипты запуска, сборки и обслуживания. Разложены по платформам: `Linux/`, `MacOS/` (`.command`), `Windows/` (`.bat`).
На macOS скрипты имеют расширение `.command` (запуск двойным кликом в Finder). Если после распаковки не запускаются: `chmod +x Empire.Scripts/MacOS/*.command`.
Запускать можно откуда угодно — скрипты сами переходят в корень проекта.
Режимы: `Debug | DebugGame | Development (по умолчанию) | Test | Shipping`. Доп. опции: `EMPIRE_CONFIG_FLAGS="--editor=n --samples=y"`.

| Скрипт | Что делает |
|---|---|
| `Setup` | установка xmake |
| `Configure [Режим]` | только конфигурация |
| `Build [Режим] [Таргет]` | сборка |
| `Rebuild [Режим] [Таргет]` | clean + сборка |
| `Clean [All]` | очистка (`All` — ещё `Empire.Build`, `.xmake`) |
| `Run [Game\|Client\|Server\|Editor\|Tests] [Режим] [арг...]` | универсальный запуск |
| `RunGame / RunClient / RunServer / RunEditor` | короткие обёртки над `Run` |
| `RunTests` | тесты (режим Test) |
| `RunSamples [Режим]` | сборка с `--samples=y` |
| `RunDedicated [порт] [Режим]` | выделенный сервер, Shipping, без редактора |
| `RunClientServer [Режим]` | сервер + клиент вместе |
| `Package`, `CompileAssets`, `Docs`, `Format` | заглушки под будущее |
| `Info` | версии/окружение |

`Common.sh` / `Common.bat` (на macOS `Common.command`) — общий код, напрямую не запускать.
