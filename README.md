# Empire 1.0

Слои: Kernel → Platform → Graphics → Engine → Runtime, а также Tools и Tests.
Math стоит в стороне от этой цепочки: зависит только от Kernel (Types, Assert), от Platform и Graphics не зависит;
Engine использует Math.

```
Kernel ─┬─ Math ───────────────┐
        └─ Platform ─ Graphics ─┴─ Engine ─ Runtime
```

Namespace и имена файлов повторяют пути папок, подробнее в `ARCHITECTURE.md`.

Что где лежит:
- Kernel: `Kernel.Compiler` (встроенные функции компилятора), типы, память, контейнеры, логирование и всё, что напрямую работает с ОС и не знает про окна:
  `Kernel.System`, `Kernel.Time`, `Kernel.Threading`, `Kernel.FileSystem`, `Kernel.Process`,
  `Kernel.Library`, `Kernel.Environment`, `Kernel.Network`.
- Platform: окна и ввод (`App`, `Events`, `Window`, `Surface`, `Input`, `Cursor`, `Monitor`, `Clipboard`, `Dialog`)
  и `Audio` (разные звуковые бэкенды, как у окон), плюс `ThirdParty` для XCB/XLib.

Сборка идёт в `Empire.Build/Build.<Платформа>.<Режим>/Build.<Слой>/`
(`Empire.Engine` → `Build.Engine`), режимы друг с другом не смешиваются.

## Runtime

Точки входа (исполняемые файлы), общий код в `Runtime.Shared`:

| Цель | Запуск | Папка | Что это |
|---|---|---|---|
| `Runtime.Game` | `Empire.Game` | `Runtime.Game` | основная точка входа игры |
| `Runtime.Client` | `Empire.Client` | `Runtime.Game/Game.Client` | клиент: окно, рендер, звук, UI, сеть |
| `Runtime.Server` | `Empire.Server` | `Runtime.Game/Game.Server` | выделенный сервер без окна, рендера и звука |
| `Runtime.Standalone` | `Empire.Standalone` | `Runtime.Game/Game.Standalone` | клиент и сервер в одном процессе (peer-to-peer, client-host), режим задаётся аргументами |
| `Runtime.Editor` | `Empire.Editor` | `Runtime.Editor` | редактор (`--editor=n` отключает), подпапки `Editor.*` собираются в один файл |

`Empire.*` это короткие имена запуска (`xmake run Empire.Editor`), они просто запускают соответствующий `Runtime.*`.
Под отладчиком запускать напрямую: `xmake run -d Runtime.Game`.

## Сборка

```
xmake f -c -m release            # Release, без тестов (по умолчанию)
xmake f -c -m debug --tests=y    # Debug вместе с Empire.Tests
xmake f -c -m release --editor=n --tools=n   # без редактора и утилит
xmake
```

Опции: `--tests=y|n` (по умолчанию n), `--editor=y|n` (y), `--tools=y|n` (y).
Режим задаётся через `-m debug|release`. После смены опций или режима запускай `xmake f -c`.

## Запуск

```
xmake run Empire.Release         # Release-сборка и запуск Empire.Game
xmake run Empire.Debug           # Debug-сборка и запуск Empire.Game под отладчиком (gdb/lldb)
xmake run Empire.Game            # запуск в текущем режиме
xmake run Empire.Client
xmake run Empire.Server
xmake run Empire.Standalone --host      # аргументы после имени цели
xmake run Empire.Editor

xmake run Empire.Test            # Unit и Integration (нужен --tests=y)
xmake run Empire.Bench           # бенчмарки (нужен --tests=y)
xmake run Empire.Tests.Unit      # отдельный тест
```
