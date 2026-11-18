# Empire.Game.Engine

Структура проекта описана в `Empire.Docs/Docs.Architecture/Structure.md`.

## Быстрый старт

```bash
Empire.Scripts/Linux/Build.sh                   # Development, всё
Empire.Scripts/Linux/Build.sh Shipping          # другой режим
Empire.Scripts/Linux/Build.sh Debug Runtime.Game
Empire.Scripts/Linux/RunGame.sh                 # собрать и запустить игру
Empire.Scripts/Linux/RunServer.sh Shipping
Empire.Scripts/Linux/RunTests.sh
```

macOS — `Empire.Scripts/MacOS/*.command`, Windows — `Empire.Scripts/Windows/*.bat` (те же имена и аргументы).
Полный список — `Empire.Scripts/README.md`. Можно и напрямую: `xmake build`, `xmake run Runtime.Game`.

## Режимы сборки

| Режим | Оптимизация | Символы | Редактор |
|---|---|---|---|
| `Debug` | нет | да | да |
| `DebugGame` | движок оптимизирован, `Runtime` без оптимизаций | да | да |
| `Development` (по умолчанию) | да | да | да |
| `Test` | да | да | да, плюс тесты |
| `Shipping` | максимальная + LTO | нет | нет |

Макросов нет: конфигурации различаются только флагами оптимизации и набором собираемых модулей.

## Опции

`xmake f --editor=n --tests=y --samples=y --vulkan=y --opengl=n ...`
(или через `EMPIRE_CONFIG_FLAGS` в `Build.*`/`Run.*`).

## Таргеты

- `Runtime.Game`, `Runtime.Client`, `Runtime.Server` — точки входа (`Game.Launcher` и т.д.). Код, специфичный для конфигурации,
  лежит в `Game.Debug`, `Game.Development`… и собирается только в своём режиме; общий код — в `Game.Shared`.
- `Runtime.Editor` — только если `--editor=y` и режим не Shipping (`Nodes.Editor` — аналогично).
- `Runtime.Tests` — в режиме `Test` или с `--tests=y`; `xmake test` запускает все `Tests.*`.
- `Empire.Samples` — с `--samples=y`.

## Результаты сборки

`Empire.Build/Build.<Платформа>.<Режим>/Build.{Engine,Runtime,Content,Docs}/`, объектники в `.Obj/`.
`Empire.Samples` складываются в `Build.Runtime`.

## Бэкенды

Модули с бэкендами (Kernel: Time/Threading/Process/FileSystem/Network/Library/Environment/System, Math.Simd,
Platform.*, Graphics.*) — один таргет на модуль. В корне такого модуля только `xmake.lua` (без `.hpp`/`.cpp`),
каждый бэкенд лежит в подпапке `<Модуль>.<Бэкенд>/` (`xmake.lua` + `<Бэкенд>.<Модуль>.cpp`) и добавляет файлы в таргет модуля.
Какие бэкенды собираются — решает корневой `xmake.lua` модуля (`is_plat`, `is_arch`, `has_config`).
`Platform.ThirdParty` — headeronly, бэкенды там только `.hpp`.

## Соглашения

- В каждой папке-модуле: `xmake.lua`, `<Имя>.hpp`, `<Имя>.cpp` (для бинарников — `Main.cpp`).
- Папки с вложенными модулями — только `includes(...)`.
- Пространства имён: `DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTime`.
  Имена, начинающиеся с цифры (`2D.Sprite2D`), получают префикс `Nodes`.
  В заголовках только `#pragma once`, в `.cpp` — `#include` собственного заголовка.
- Ни макросов, ни `add_deps` между модулями нет: связи добавляются позже, по мере появления кода.
- Совпадающие имена таргетов получают префикс родителя (например `Runtime.Editor.Plugin` и `Nodes.Editor.Plugin`).
