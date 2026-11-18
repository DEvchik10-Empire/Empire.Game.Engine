# Make — система збирання Empire Game Engine

Make збирає та запускає три окремі продукти: **MakeTool**, **Engine** і **Game**.
Скрипти: `Make.sh` (Linux), `Make.command` (macOS), `Make.bat` (Windows).

## Швидкий старт

Виконуйте команди окремо, у такому порядку:

```bash
# 1. Один раз зібрати MakeTool для поточної ОС
./Empire.Scripts/Make.sh Bootstrap Development

# 2. Зібрати потрібний продукт і конфігурацію
./Empire.Scripts/Make.sh Build Engine Development -j8
./Empire.Scripts/Make.sh Build Game DebugGame -j8
```

Запуск скрипта без аргументів показує ці короткі приклади. `Help` показує повнішу довідку.

## Головне правило

Для `Build` і `Rebuild` обов'язково вказати **один продукт** і **один режим**:

```text
Build [Платформа] Продукт Режим [параметри] [цілі]
```

| Продукт | Доступні режими |
|---|---|
| `Engine` | `Debug`, `Development`, `Test`, `Shipping` |
| `Game` | `Debug`, `DebugGame`, `Development`, `Test`, `Shipping` |
| `MakeTool` | `Debug`, `Development`, `Test`, `Shipping` |

Якщо продукт або режим пропущений, команда завершується помилкою. Make не запускає всі продукти автоматично.

- `Engine` збирає лише Xmake-групу `Engine`.
- `Game` збирає лише Xmake-групу `Game`.
- `MakeTool` збирає лише MakeTool.
- `DebugGame` доступний лише для Game. Ігрові модулі збираються в `DebugGame`, Engine — у `Development`.
- `Release` — псевдонім `Shipping`.

## MakeTool та Engine/Game

MakeTool — інструмент збірки, тому він завжди збирається для ОС, на якій запущений Make. Не додавайте до `MakeTool` назви цільових платформ.

Engine/Game використовують будь-яку наявну збірку MakeTool для host-ОС. Режим MakeTool не зобов’язаний збігатися з режимом Engine/Game. Якщо MakeTool ще не зібраний, виконайте:

```bash
./Empire.Scripts/Make.sh Bootstrap Development
```

MakeTool може бути зібраний у Debug, Development, Test або Shipping. Make автоматично знаходить доступний host-бінарник; Development має пріоритет, але не є обов’язковим.

## Режими коротко

- `Debug` — збірка для налагодження без оптимізації.
- `Development` — основний режим розробки.
- `Test` — режим для збирання та запуску тестів.
- `DebugGame` — лише Game; Engine залишається в Development.
- `Shipping` — оптимізована збірка для розповсюдження; редактор вимкнений.
- `Release` — те саме, що `Shipping`.

MakeTool отримує вибраний режим у власній конфігурації. Точні прапорці його компіляції визначаються кодом Tools.Make.

## Платформи та параметри

Платформи Engine/Game: `Linux`, `Windows`, `MacOS`, `Android`, `iOS`, `WASM`. Для кросзбирання потрібні відповідні SDK, компілятори й toolchain.

| Параметр | Значення |
|---|---|
| `-j8` або `-j 8` | Кількість паралельних задач |
| `-Verbose` / `-Quiet` | Докладний / скорочений вивід (не застосовуються до MakeTool) |
| `-Force` | Примусове перебудування цілі |
| `-Editor` / `-NoEditor` | Увімкнути / вимкнути редактор |
| `-Tests` | Додати тестові цілі поза режимом Test |
| `-Samples` | Додати `Empire.Samples` |
| `-Port=7777` | Порт для `RunDedicated` |
| `-All` | З `Clean` видалити `Empire.Build`, `.MakeTool` і `build` |
| `--` | Передати наступні аргументи запущеній програмі |

## Команди

| Команда | Призначення |
|---|---|
| `Build` | Зібрати вибраний продукт |
| `Rebuild` | Примусово перебудувати вибраний продукт |
| `Bootstrap` | Зібрати MakeTool безпосередньо з вихідного коду |
| `Configure` | Налаштувати проєкт без компіляції |
| `Clean` | Очистити артефакти; `-All` видаляє також build-каталоги та кеші |
| `Run`, `RunEditor`, `RunClient`, `RunServer` | Зібрати й запустити відповідну ціль |
| `RunDedicated` | Запустити виділений сервер |
| `RunClientServer` | Запустити сервер і клієнт разом |
| `RunTests` | Зібрати й запустити тести |
| `CopyContent` | Скопіювати ресурси до збірки Game |
| `Info`, `Version`, `Help` | Інформація, версії та довідка |

## Приклади

`Rebuild` примусово перебудовує лише вибраний продукт/групу; він не запускає глобальне очищення всіх продуктів.

```bash
# MakeTool — поточна ОС
./Empire.Scripts/Make.sh Build MakeTool Debug
./Empire.Scripts/Make.sh Bootstrap Development
./Empire.Scripts/Make.sh Build MakeTool Test
./Empire.Scripts/Make.sh Build MakeTool Shipping

# Engine — Linux
./Empire.Scripts/Make.sh Build Linux Engine Debug -j8
./Empire.Scripts/Make.sh Build Engine Development -j8
./Empire.Scripts/Make.sh Build Linux Engine Test -j8
./Empire.Scripts/Make.sh Build Engine Shipping -j8

# Game — Linux
./Empire.Scripts/Make.sh Build Linux Game Debug -j8
./Empire.Scripts/Make.sh Build Game DebugGame -j8
./Empire.Scripts/Make.sh Build Linux Game Development -j8
./Empire.Scripts/Make.sh Build Linux Game Test -j8
./Empire.Scripts/Make.sh Build Game Shipping -j8

# Інші операції
./Empire.Scripts/Make.sh Rebuild Engine Shipping -j8
./Empire.Scripts/Make.sh RunEditor -Development
./Empire.Scripts/Make.sh RunTests
./Empire.Scripts/Make.sh RunDedicated -Shipping -Port=7777
./Empire.Scripts/Make.sh Clean
./Empire.Scripts/Make.sh Clean -All
./Empire.Scripts/Make.sh Info
./Empire.Scripts/Make.sh Version
```

## Каталоги результатів

Шаблон: `Empire.Build/Build.<Платформа>.<Продукт>.<Режим>/`.

Наприклад:

```text
Empire.Build/
├── Build.Linux.Engine.Development/
├── Build.Linux.Engine.Shipping/
├── Build.Linux.Game.DebugGame/
├── Build.Linux.Game.Shipping/
└── Build.Linux.MakeTool.Development/
```

Для `DebugGame` Engine використовує каталог `Build.<Платформа>.Engine.Development`.

## Версії

Публічні версії задаються на початку кореневого `make.lua`:

```lua
EngineVersion = "v2.1.0-Dev-26.11.18"
MakeToolVersion = "v1.0.0-Dev-26.11.18"
```

Це незалежні публічні версії. `set_version("2.1.0")` — внутрішня числова версія Xmake-проєкту.


Приклад збирання MakeTool у режимі Shipping: `./Empire.Scripts/Make.sh -bs Shipping`.


Назви продуктів, режимів і платформ — звичайні значення без дефіса: `Build Engine Shipping`, `Build Linux Game DebugGame`. Дефіс використовується для параметрів і перемикачів на кшталт `-j8`, `-Editor` та `-Force`. Старі варіанти з дефісами перед продуктом/режимом залишені для сумісності.
