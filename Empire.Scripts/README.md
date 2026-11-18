# Empire.Scripts

Скрипты запуска, сборки и обслуживания. Разложены по платформам: `Linux/`, `MacOS/` (`.command`), `Windows/` (`.bat`).
На macOS скрипты имеют расширение `.command` (запуск двойным кликом в Finder). Если после распаковки не запускаются: `chmod +x Empire.Scripts/MacOS/*.command`.
Запускать можно откуда угодно — скрипты сами переходят в корень проекта.
Режимы: `Debug | DebugGame | Development (по умолчанию) | Test | Shipping`. Доп. опции: `EMPIRE_CONFIG_FLAGS="--editor=n --samples=y"`.

| Скрипт | Что делает |
|---|---|
| `Configure [Режим]` | только конфигурация |
| `Build [Режим] [Таргет]` | сборка, затем копирует Docs, Content и README в папку сборки |
| `BuildDevelopment` | версия для разработки (= `Build Development`) |
| `BuildShipping` | релизная версия (= `Build Shipping`) |
| `CopyContent [Режим]` | только скопировать `Empire.Docs`, `Empire.Content` и `README.md` в `Empire.Build/Build.<Платформа>.<Режим>/` (`Build.Docs`, `Build.Content`) |
| `Rebuild [Режим] [Таргет]` | clean + сборка |
| `Clean [All]` | очистка (`All` — ещё `Empire.Build`, `.xmake`) |
| `Run [Game\|Client\|Server\|Editor\|Tests] [Режим] [арг...]` | универсальный запуск |
| `RunGame / RunClient / RunServer / RunEditor` | короткие обёртки над `Run` |
| `RunTests` | тесты (режим Test) |
| `RunDedicated [порт] [Режим]` | выделенный сервер, Shipping, без редактора |
| `RunClientServer [Режим]` | сервер + клиент вместе; при выходе клиента сервер останавливается |
| `Info` | версии/окружение |

`Common.sh` / `Common.bat` (на macOS `Common.command`) — общий код, напрямую не запускать.
