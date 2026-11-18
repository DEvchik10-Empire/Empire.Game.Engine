 Make — система збирання Empire Game Engine

Make — командний інтерфейс рушія для підготовки конфігурації, збирання цілей, запуску програм, тестування та очищення результатів. Для різних ОС використовуються окремі файли запуску, але назви команд і параметрів однакові:

- `Make.sh` — Linux;
- `Make.command` — macOS;
- `Make.bat` — Windows.

Усі повідомлення та ця довідка українською. Назви команд, режимів, параметрів, цілей і технічних інструментів залишаються англійськими, щоб їх можна було однаково використовувати в автоматизації.

## Швидкий старт

Виконуйте команди з кореневого каталогу `Empire.Game.Engine`.

### Linux

```bash
# 1. Одноразово зібрати MakeTool, якщо його ще немає
./Empire.Scripts/Make.sh Bootstrap

# 2. Зібрати рушій для розробки
./Empire.Scripts/Make.sh Build -Development -j8

# 3. Запустити редактор
./Empire.Scripts/Make.sh RunEditor -Development
```

### macOS

```bash
./Empire.Scripts/Make.command Bootstrap
./Empire.Scripts/Make.command Build -Development -j8
./Empire.Scripts/Make.command RunEditor -Development
```

### Windows

```bat
Empire.Scripts\\Make.bat Bootstrap
Empire.Scripts\\Make.bat Build -Development -j8
Empire.Scripts\\Make.bat RunEditor -Development
```

`Bootstrap` потрібен для початкового збирання самого MakeTool. Після цього Make автоматично знаходить його в `Empire.Build/Build.<Платформа>.MakeTool/` і використовує розміщений поруч каталог `Make.Runtime`.

## Синтаксис

```text
Make.<sh|command|bat> <Команда> [Параметри] [Ціль] [-- АргументиПрограми]
```

- **Команда** визначає дію: `Build`, `Clean`, `Run` тощо.
- **Параметри** задають режим, платформу, паралельність та додаткові опції.
- **Ціль** — необов'язкова назва окремого модуля/програми, наприклад `Runtime.Game`.
- `--` завершує обробку параметрів Make. Усе після нього передається запущеній програмі.

Команди й параметри слід писати саме так, як наведено в прикладах. Не додавайте пробіл між `-j` і числом, якщо використовуєте короткий запис `-j8`; обидва варіанти `-j8` та `-j 8` підтримуються.

## Команди

| Команда | Скорочення | Призначення |
|---|---|---|
| `Bootstrap` | `-bs` | Зібрати MakeTool із вихідного коду та підготувати його runtime. |
| `Configure` | `-cf` | Створити або оновити конфігурацію збирання. Не компілює цілі. |
| `Build` | `-b` | Налаштувати проєкт і зібрати всі цілі, передбачені конфігурацією, або одну/кілька вказаних цілей. |
| `Rebuild` | `-rb` | Налаштувати проєкт, очистити попередні результати MakeTool і зібрати цілі знову. |
| `Clean` | `-c` | Очистити згенеровані результати збирання через MakeTool. З `-All` додатково видаляється весь `Empire.Build`. |
| `CopyContent` | `-cc` | Скопіювати `Empire.Docs`, `Empire.Content` і кореневий `README.md` до каталогу поточної збірки. |
| `Run` | `-r` | Зібрати й запустити `Runtime.Game` або вказану ціль. |
| `RunEditor` | `-re` | Зібрати й запустити `Runtime.Editor`. |
| `RunClient` | `-rc` | Зібрати й запустити `Runtime.Client`. |
| `RunServer` | `-rs` | Зібрати й запустити `Runtime.Server`. |
| `RunDedicated` | `-rd` | Зібрати й запустити виділений сервер; стандартний порт — `7777`. |
| `RunClientServer` | `-rcs` | Зібрати сервер і клієнт, запустити сервер у фоні та клієнт у поточному процесі. |
| `RunTests` | `-t` | Перемкнутися на `Test`, зібрати проєкт і запустити тести. |
| `Info` | `-i` | Показати ОС, архітектуру, процесор, кількість потоків, RAM, відеокарту, вільне місце та шляхи проєкту. |
| `Version` | — | Показати версії Empire Game Engine і MakeTool. Не показує версію скриптів. |
| `Help` | `-h` | Показати довідку в терміналі. |

Якщо команда не вказана, використовується `Help`.

## Режими збирання

Режим визначає оптимізацію, наявність налагоджувальних символів та склад цілей. Режими зберігаються в окремих каталогах, тому результати `Debug`, `Development` і `Shipping` не змішуються.

| Параметр | Оптимізація | Налагоджувальні символи | Призначення |
|---|---|---|---|
| `-Debug` | `none` (`-O0`) | Так | Налагодження коду рушія без оптимізацій компілятора. |
| `-DebugGame` | Рушій — `faster`; шар `Runtime` — без оптимізації | Так | Налагодження ігрового коду на тлі оптимізованого рушія. |
| `-Development` | `fast` | Так | Основний режим щоденної розробки. |
| `-Test` | Як `Development` | Так | Збирання з тестовими цілями; використовується для запуску тестів. |
| `-Shipping` | `fastest` + LTO | Символи приховуються та видаляються | Фінальна оптимізована збірка. Редактор не збирається. |
| `-Release` | Такий самий, як `Shipping` | Так само, як `Shipping` | Зручний псевдонім `-Shipping`. |

**Режим за замовчуванням — `Development`.** Якщо режим не передати, Make використовує його автоматично.

### Коли який режим використовувати

- `Debug` — коли потрібно налагоджувати сам рушій і важливо не приховувати помилки оптимізацією.
- `DebugGame` — коли потрібно налагоджувати код гри, але не хочеться збирати весь рушій без оптимізацій.
- `Development` — звичайна робота над рушієм, редактором та грою.
- `Test` — перевірка тестових цілей; для стандартного сценарію використовуйте команду `RunTests`.
- `Shipping` або `Release` — збірка для розповсюдження чи фінального запуску, а не для звичайного налагодження.

### Приклади

```bash
# Налаштувати Debug
./Empire.Scripts/Make.sh Configure -Debug

# Зібрати Debug
./Empire.Scripts/Make.sh Build -Debug -j8

# Зібрати гру в DebugGame
./Empire.Scripts/Make.sh Build -DebugGame Runtime.Game

# Звичайна розробницька збірка
./Empire.Scripts/Make.sh Build -Development -j8

# Фінальна збірка — обидва варіанти еквівалентні
./Empire.Scripts/Make.sh Build -Shipping -j8
./Empire.Scripts/Make.sh Build -Release -j8

# Повністю перезібрати Shipping
./Empire.Scripts/Make.sh Rebuild -Release -j8
```

`Configure` лише готує конфігурацію. Для компіляції після нього потрібна команда `Build`.

## Платформи

| Параметр | Цільова платформа |
|---|---|
| `-Linux` | Linux |
| `-Windows` | Windows |
| `-MacOS` | macOS |
| `-Android` | Android |
| `-iOS` | iOS |
| `-WASM` | WebAssembly |

Якщо платформу не вказано, використовується платформа поточної системи. Параметр платформи вибирає ціль, але не встановлює SDK або компілятор. Для кросзбирання заздалегідь встановіть відповідний toolchain, SDK та залежності. Наявність параметра не означає, що кожна ціль уже підтримує кожну платформу.

Приклад:

```bash
./Empire.Scripts/Make.sh Configure -Windows -Development
./Empire.Scripts/Make.sh Build -Windows -Development -j8
```

## Додаткові параметри

| Параметр | Значення |
|---|---|
| `-j8` або `-j 8` | Кількість паралельних задач. Якщо не вказано, використовується кількість доступних процесорних потоків. |
| `-Verbose` | Увімкнути докладніший вивід MakeTool. |
| `-Quiet` | Зменшити кількість повідомлень MakeTool. |
| `-Force` | Примусово перебудувати ціль. |
| `-Editor` | Увімкнути збирання редактора. Типово увімкнено; у `Shipping` редактор усе одно вимкнений. |
| `-NoEditor` | Вимкнути збирання редактора. |
| `-Tests` | Додати тестові цілі поза режимом `Test`. |
| `-Samples` | Додати цілі з `Empire.Samples`. |
| `-Port=7777` або `-Port 7777` | Встановити порт для `RunDedicated`. Допустимий діапазон — `1`–`65535`. |
| `-All` | Спеціальний параметр для `Clean`: видалити весь каталог `Empire.Build`. |
| `--` | Передати всі наступні аргументи програмі, яку запускає `Run` або відповідна команда запуску. |

Параметри можна комбінувати, наприклад:

```bash
./Empire.Scripts/Make.sh Build -Development -Editor -Samples -j8
./Empire.Scripts/Make.sh Build -Development -Tests -NoEditor
./Empire.Scripts/Make.sh RunDedicated -Shipping -Port=9000
```

## Цілі рушія

Найчастіше використовуються такі цілі:

| Ціль | Призначення |
|---|---|
| `Runtime.Game` | Основна програма гри. |
| `Runtime.Client` | Клієнтська програма. |
| `Runtime.Server` | Серверна програма. |
| `Runtime.Editor` | Редактор рушія; не збирається в `Shipping`. |
| `Runtime.Tests` | Тестові цілі. |

Можна вказувати кілька цілей. Якщо ціль не вказана, `Build` збирає набір, який підключено кореневою конфігурацією `make.lua` з урахуванням режиму й опцій.

```bash
# Одна ціль
./Empire.Scripts/Make.sh Build Runtime.Game -Development

# Декілька цілей
./Empire.Scripts/Make.sh Build Runtime.Client Runtime.Server -Development
```

## Запуск програм і аргументи

```bash
# Основна гра
./Empire.Scripts/Make.sh Run -Development

# Вказана ціль
./Empire.Scripts/Make.sh Run Runtime.Game -Development

# Редактор, клієнт або сервер
./Empire.Scripts/Make.sh RunEditor -Development
./Empire.Scripts/Make.sh RunClient -Development
./Empire.Scripts/Make.sh RunServer -Development

# Виділений сервер
./Empire.Scripts/Make.sh RunDedicated -Shipping -Port=7777

# Сервер і клієнт разом
./Empire.Scripts/Make.sh RunClientServer -Development

# Аргументи після -- передаються самій програмі
./Empire.Scripts/Make.sh Run Runtime.Game -Development -- -windowed
```

## Тести та приклади

`RunTests` автоматично вибирає режим `Test`, збирає цілі й запускає тестовий механізм MakeTool:

```bash
./Empire.Scripts/Make.sh RunTests
```

Щоб додати тести до іншого режиму:

```bash
./Empire.Scripts/Make.sh Build -Development -Tests
```

Приклади з `Empire.Samples` додаються параметром `-Samples`:

```bash
./Empire.Scripts/Make.sh Build -Development -Samples
```

## Очищення та повторне збирання

```bash
# Очистити результати через MakeTool
./Empire.Scripts/Make.sh Clean

# Видалити весь Empire.Build, включно з MakeTool та кешами
./Empire.Scripts/Make.sh Clean -All

# Очистити результати й зібрати конфігурацію заново
./Empire.Scripts/Make.sh Rebuild -Development -j8
```

**Увага:** `Clean -All` видаляє весь `Empire.Build`, а отже, також зібраний MakeTool. Після цього для наступного використання інструмента знову виконайте `Bootstrap`.

## Де зберігаються результати

Загальний шаблон:

```text
Empire.Build/
├── Build.<Платформа>.MakeTool/
│   ├── MakeTool
│   └── Make.Runtime/
└── Build.<Платформа>.<Режим>/
    ├── Build.Engine/
    ├── Build.Runtime/
    └── .Obj/
```

Наприклад, Linux-збірки `Development` і `Shipping` розташовуються окремо:

```text
Empire.Build/Build.Linux.Development/
Empire.Build/Build.Linux.Shipping/
```

Бібліотеки та виконувані файли розподіляються за шарами `Build.Engine` і `Build.Runtime`. Проміжні об'єктні файли зберігаються в `.Obj`. MakeTool має власний каталог, окремий від результатів рушія.

## Інформація про систему та версії

```bash
./Empire.Scripts/Make.sh Info
./Empire.Scripts/Make.sh Version
```

`Info` показує доступні системні характеристики. Частина відомостей, зокрема назва відеокарти, залежить від наявності системних утиліт (`lspci`, `glxinfo` або `system_profiler`).

`Version` показує версії рушія та MakeTool. Версія `Empire.Scripts` не є версією рушія чи інструмента й окремо цією командою не виводиться.

## Вимоги для Bootstrap

Для збирання MakeTool із вихідного коду потрібні:

- C-компілятор і стандартні системні заголовки;
- `sh`;
- GNU Make;
- усі вихідні залежності, що постачаються разом із репозиторієм.

На Windows потрібне середовище, яке надає `sh` і `make` (наприклад, MSYS2/MinGW). Для збирання самого рушія додатково потрібні компілятор C++, SDK та залежності відповідної цільової платформи.

## Конфігураційні файли

Кореневий файл конфігурації рушія — `make.lua`. Він задає версію проєкту, стандарт C++, дозволені режими, опції та підключення модулів. Вкладені `xmake.lua` залишаються з такою назвою для сумісності з механізмом `includes()` MakeTool.

Довідка командного рядка доступна без відкриття цього документа:

```bash
./Empire.Scripts/Make.sh Help
```
