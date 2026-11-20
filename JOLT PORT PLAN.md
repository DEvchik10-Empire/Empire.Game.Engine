# План переноса JoltPhysics 5.7.0 в Empire Game Engine

Документ для людей и агентов. Читать целиком перед тем, как переносить любой файл.
Состояние на 2026-11-19. Источник: архив JoltPhysics-5_7_0 (папка `Jolt/`) против архива Empire_Game_Engine.

**Версия 2 (правки по замечаниям).** Что изменилось в правилах:
- Макросов нет совсем (ни `JPH_*`, ни экспортных `*_API`/`*_EXPORT`); неустранимое для компилятора живёт только в `Kernel.Compiler` (1.1, 1.10). В текущем архиве `*_API` остались в заглушках Math/Graphics/Engine/Runtime и `_EXPORT` в корневом `xmake.lua`, их надо убрать (1.3).
- Никакого C-стиля (1.11).
- Типы Kernel подключаются `using namespace EmpireKernel::KernelTypes;` и пишутся без префикса (1.2).
- Перенесённый код становится родным, а лучшее из Jolt (включая математику) заменяет движковое, с доработкой под архитектуру Empire; 2D/3D-структура сохраняется, 3D-код Jolt идёт в `*.3D` (1.12, разделы 3 и 4).

Условные обозначения:
- `[x]` перенесено (в Empire есть порт, сделанный из Jolt)
- `[~]` перенесено частично или сделано по-своему (в скобках что именно)
- `[ ]` не перенесено
- `[-]` переносить не нужно (в скобках почему)
- Числа после имени файла Jolt это строки в оригинале (оценка объёма).

Функциональный перенос и проверки Linux x86_64 описаны в контрольной точке 2.6. Остальные платформы и полная построчная сверка всех исходников не подтверждены.

------------------------------------------------------------------------------------------------------------------------

## 0. Итог в трёх строках

1. **Ядро:** применимые non-Math компоненты `Jolt/Core` портированы в `Empire.Kernel`. Отличия API допустимы только при сохранении исходной функциональности; все адаптации описаны в таблице 2.1. Компиляция на ARM64/RISC-V и других платформах не входит в критерий завершения по решению пользователя. После Linux-сборок и тестов пользователь разрешил начать Math; проверка на основной платформе остаётся за пользователем.
2. **Математика:** начат порт Jolt в `Empire.Math`: добавлена часть scalar utilities, storage-векторы и базовые изменения `Vector3/4`. Это пока не SIMD-порт и не завершённая замена; прежние реализации и заглушки SIMD ещё остаются. Jolt-математика должна **заменить** движковую, а не лежать рядом; потребителей мало (раздел 3).
3. **Физика:** не перенесено ничего. `Engine.Physics` это пустой скелет (по 21 строке на заголовок, только namespace) (раздел 4).

Порядок работ: ядро (раздел 2) -> математика (раздел 3) -> геометрия -> физика (раздел 4). Физика не соберётся, пока нет FP-управления,
SharedMutex, Semaphore, SIMD-типов и геометрических алгоритмов.

------------------------------------------------------------------------------------------------------------------------

## 1. ГАЙД: как переносить (правила Empire)

### 1.1 Главные принципы

1. **Это переписывание под архитектуру Empire, а не копирование.** Логика, порядок операций и численное поведение остаются как в Jolt.
   Меняется всё остальное: имена, namespace, типы, контейнеры, разбиение на файлы, а главное стиль: никаких макросов и никакого C-стиля (пп. 2 и 3).
2. **Макросов нет.** Ни `#define` (константы, функции, экспорт, `JPH_*`), ни `#ifdef`/`#if` в обычном коде. Всё, что в Jolt делал макрос, делается языком:
   `inline constexpr`, `inline`/шаблоны, `if constexpr`, аргументы по умолчанию, RAII-объекты, файл бэкенда, выбранный xmake (полная таблица в 1.10).
   Если без препроцессора обойтись нельзя и этого требует компилятор или процессор (`NDEBUG`, `__SSE2__`, `__builtin_*`, интринсики), оно живёт **только в `Kernel.Compiler`**
   и наружу выходит обычными `constexpr`-значениями и функциями. Допустимы только `#pragma once` и `#include`. Экспортных макросов (`*_API`, `*_EXPORT`) нет нигде (1.3).
3. **C-стиля нет.** Это не «Jolt, переименованный в PascalCase», а код на современном C++ в стиле Empire: см. список запретов в 1.11.
4. **После переноса код родной.** Файл, перешедший границу в Empire, ничем не отличается от соседних: нет `JPH`-префиксов, нет слоя совместимости, нет «типов Jolt рядом с типами движка».
   Источник остаётся только в комментарии-шапке и в `THIRD_PARTY_NOTICES.md` (лицензия).
5. **Что в Jolt лучше, то заменяет движковое.** Если реализация Jolt лучше (SIMD-математика, геометрия, контейнеры), она целиком заменяет движковый аналог, даже если это математика,
   и дорабатывается под архитектуру Empire. Правила замены в 1.12. Чего в Jolt нет, то остаётся как есть.
6. **Численное поведение не трогать.** Не «упрощать» выражения, не менять порядок сложения, не заменять свои sin/cos на libm, не включать FMA.
   Всё, что в Jolt выглядит лишним, часто нужно для детерминизма. Менять можно только после теста, что результат совпадает бит в бит.
7. **В Kernel нет `std`.** Ни `std::`, ни системных заголовков (кроме `System.Linux.cpp` и подобных бэкендов). Всё берётся из `Kernel*` (см. 1.4).
   Math и Physics тоже по возможности без `std` (`<cmath>`, `<cstring>` не подключать, брать из Kernel/своё). Заголовок интринсиков компилятора (`immintrin.h`, `arm_neon.h`) допустим только в `Kernel.Compiler`.
8. **Исключений Jolt не использует.** Не добавлять `throw` в порт, особенно в горячие пути физики. Ошибки это `Bool`/`Result`/assert, как в оригинале.
9. **Предупреждения = ошибки** (`set_warnings("all", "error")` в корневом `xmake.lua`). Порт обязан собираться без warning на gcc и clang. Подавлять предупреждения нельзя (1.10, `JPH_SUPPRESS_WARNING`).
10. **Лицензия.** Jolt под MIT. Не удалять копирайт. Положить текст лицензии в `THIRD_PARTY_NOTICES.md` (версия: Jolt 5.7.0).
    В каждом портированном файле оставлять комментарий-источник (см. 1.6).
11. **Каждый перенесённый файл получает тест** (см. 1.8). Нет теста = не считается перенесённым.

### 1.2 Структура папок и namespace

Один Jolt-файл не равен одному Empire-файлу по имени, но равен по смыслу. Правило имён:

| Что | Правило | Пример |
|---|---|---|
| Папка модуля | `Empire.<Слой>/<Слой>.<Модуль>/` | `Empire.Kernel/Kernel.Memory/` |
| Файл | `<Модуль>.<Имя>.hpp` / `.cpp` | `Memory.Reference.hpp`, `Jobs.JobSystemThreadPool.cpp` |
| Платформенный бэкенд | папка `<Модуль>.<Платформа>/`, файл `<Платформа>.<Модуль>.cpp` | `Kernel.Threading/Threading.Linux/Linux.Threading.cpp` |
| Include | всегда от корня проекта | `#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"` |
| Namespace | повторяет путь папок | см. ниже |

Namespace (отступ 4 пробела на уровень, скобки с новой строки, как в соседних файлах):

```cpp
namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel      // слой: EmpireKernel / EmpireMath / EmpirePlatform / EmpireEngine
        {
            namespace KernelMemory  // модуль: Kernel + имя папки
            {
                // код
            }
        }
    }
}
```

- Math: `EmpireMath::MathVector`, `EmpireMath::MathGeometry` и т.д.
- Engine: `EmpireEngine::EnginePhysics::PhysicsShape::Shape3D` (подмодуль и размерность 2D/3D это вложенные namespace).
- **Типы Kernel подключаются директивой `using namespace EmpireKernel::KernelTypes;` и пишутся без префикса: `UInt32`, `Float`, `Size`, `Bool`.**
  Так сделано в `Memory.cpp` (директива на уровне `EmpireGameEngine`). В заголовке директива стоит внутри namespace модуля, не в глобальной области.
  Это правило для любого слоя, в том числе Math и Engine. Остальные модули Kernel пишутся с префиксом модуля: `KernelAssert::DebugCheck`, `KernelSync::Atomic`, `KernelContainers::Array`.
  Старый приём `using namespace EmpireKernel;` + `KernelTypes::Float` в новом и переписываемом коде не используется.
- Код внутри класса: модификаторы доступа (`public:`) отступом на уровень глубже `class`, члены ещё на уровень глубже. Копировать форму соседних файлов.

### 1.3 Сборка (xmake)

Каждый модуль это target с `add_headerfiles("*.hpp")`, `add_files("*.cpp")`, `add_deps(...)` и `-fvisibility=default`. Образец (`Kernel.Sync/xmake.lua`):

```lua
target("Kernel.Sync")
    add_headerfiles("*.hpp")
    add_files("*.cpp")
    add_deps("Kernel.Compiler", "Kernel.Types", "Kernel.System")
    add_cxxflags("-fvisibility=default", {tools = {"gcc", "clang"}})
```

Правила:
1. Новый модуль надо добавить в `includes(...)` родительского `xmake.lua` (`Empire.Kernel/xmake.lua`, `Empire.Math/xmake.lua`, `Empire.Engine/Engine.Physics/xmake.lua`).
2. Зависимости (`add_deps`) только вниз по слоям: Kernel <- Math <- Platform/Graphics <- Engine <- Runtime. Kernel не знает про Math, Math не знает про Engine.
3. **Экспорта через макросы нет.** Корень задаёт `-fvisibility=hidden`, а target, чьё API нужно другим `.so`, задаёт `-fvisibility=default` (как `Kernel.Sync` выше).
   На Windows экспорт символов делает xmake (правило `utils.symbols.export_all` с `{export_classes = true}`, проверить на xmake 3.1.1), а не `__declspec` в коде.
   Kernel уже так и устроен (проверено: препроцессор есть только в `Kernel.Compiler/Compiler.hpp`). Остальные слои надо привести к тому же, **убрав**:
   - блоки `#define MODULE_NAME_API ... __declspec/__attribute__` из заголовков (нашлись в Math.*, Graphics.*, Engine.*, Runtime.Shared, например `Simd.hpp`, `Device.hpp`, `Quaternion.hpp`, `Color.hpp`);
   - строку `target:add("defines", ... .. "_EXPORT")` из правила `Empire.Module` в корневом `xmake.lua`.
4. Выбор бэкенда по платформе и архитектуре делает xmake (какие файлы добавить), как сейчас `Kernel.System/System.Linux`. Ветвлений `#if` по платформе в коде нет.
5. **Горячий код (SIMD-типы, векторы, матрицы, контейнеры) обязан быть inline в заголовках.** Нельзя, чтобы каждая операция `Vector3 + Vector3` была вызовом в `.so`.
   Inline-код компилируется в вызывающем модуле, поэтому экспорт ему не нужен. Реализации SIMD должны быть `.hpp`, а не `.cpp`-заглушки с отдельным `.so` на бэкенд.
6. Тесты это отдельные binary-таргеты (`set_kind("binary")`), см. 1.8.

### 1.4 Таблица замены: Jolt -> Empire

Типы (`Kernel.Types/Types.hpp`, namespace `KernelTypes`, в коде пишутся без префикса, см. 1.2):

| Jolt | Empire | | Jolt | Empire |
|---|---|---|---|---|
| `int8/16/32/64` | `Int8/16/32/64` | | `uint8/16/32/64` | `UInt8/16/32/64` |
| `float` | `Float` | | `double` | `Double` |
| `size_t` | `Size` | | `bool` | `Bool` |
| `char` | `Char` | | `uintptr_t` | `UIntPtr` |
| `nullptr_t` | `NullPointer` | | `std::source_location` | `SourceLocation` |

Инфраструктура (все макросы `JPH_*` собраны в 1.10):

| Jolt | Empire |
|---|---|
| `JPH_NAMESPACE_BEGIN/END`, `JPH::` | вложенные namespace (1.2); префиксов нет |
| `Allocate/Free/AlignedAllocate/AlignedFree` | `KernelMemory::Allocate(Size, Alignment)` / `KernelMemory::Deallocate` |
| `memcpy/memmove/memset/memcmp` | `KernelMemory::CopyBytes/MoveBytes/FillBytes/CompareBytes` |
| `Array<T>` | `KernelContainers::Array<T>` (или `FastArray<T>`) |
| `StaticArray<T,N>` | `KernelContainers::StaticArray<T,N>` |
| `UnorderedMap/Set`, `HashTable` | `KernelContainers::HashMap` / `FastHashMap` / `FastHashSet` / `FastHashTable` |
| `Ref<T>`, `RefConst<T>`, `RefTarget<T>` | `KernelMemory::Ref/RefConst/RefTarget` (уже перенесено) |
| `atomic<T>`, `memory_order_*` | `KernelSync::Atomic<T>`, `KernelSync::MemoryOrder::*` |
| `Mutex`, `lock_guard` | `KernelSync::Mutex`, `KernelSync::LockGuard` |
| `NonCopyable` | `KernelTypes::NonCopyable` |
| `function<>` | `KernelContainers::Function<>` (внимание: копии делят вызываемое, move-only лямбды не поддерживаются) |
| `Trace(...)` | `KernelLogging::...` |

Имена внутри кода (стиль Empire это PascalCase везде):

| Jolt | Empire |
|---|---|
| поле `mRefCount` | `RefCount` |
| константа `cEmbedded` | `Embedded` |
| статический метод `sFoo()` | `Foo()` (static) |
| параметр `inValue`, `outResult`, `ioBuffer` | `Value`, `Result`, `Buffer` (смысл in/out выразить типом: `const T&`, `T&`) |
| enum `EFoo`, значение `EFoo::Bar` | `enum class Foo`, значение `Foo::Bar` |
| методы `begin()/end()` | оставить в нижнем регистре (нужно для range-for) |
| комментарии `///` по-английски | комментарии по-русски, как в соседних файлах |

### 1.5 Типичные подводные камни

- `uint` (Jolt `uint`) это `unsigned int`, переводить в `UInt32`, не в `Size`.
- `alignas(JPH_VECTOR_ALIGNMENT)` и `JPH_CPU_ADDRESS_BITS` завязаны на `Core.h`: сначала завести соответствующие `constexpr` в `Kernel.Compiler`/`Kernel.Config` (1.10), потом переносить типы.
- **Макрос в Jolt мог не вычислять аргументы, функция в Empire вычисляет.** `JPH_ASSERT(Дорого())` в Release не вызывал `Дорого()`, а `DebugCheck(Дорого(), ...)` вызовет.
  Если условие дорогое или с побочными эффектами, оборачивать в `if constexpr (KernelCompiler::IsDebugBuild)`.
- Jolt-контейнеры дают гарантии, которых нет у вашего `Array`: `Array` у Jolt умеет `push_back/emplace_back/resize/pop_back/insert/erase`, у вашего по README нет `Back/Pop/Insert/Resize`.
  Перед переносом файла проверить, какие методы реально нужны, и **сначала дописать их в Kernel.Containers**, а не обходить в порте.
- У `HashMap` Empire нет итераторов (только `ForEach`), копирование запрещено. Код Jolt, который делает `for (auto& kv : map)`, нужно либо адаптировать, либо дописать итераторы в `HashMap`.
- Профайлерные макросы Jolt (`JPH_PROFILE`, `JPH_PROFILE_FUNCTION`) пока просто убирать и оставить комментарий `// TODO Profiler`; после появления `Kernel.Profiler` это RAII-объект (1.10).
- Члены классов, которые Jolt вырезает макросом (`JPH_DEBUG_RENDERER` и подобные): `constexpr` не убирает член из класса. Решать по месту: оставить член всегда либо вынести в отдельный тип.
- `JPH_CROSS_PLATFORM_DETERMINISTIC`, `JPH_DOUBLE_PRECISION`, `JPH_USE_FMADD`, `JPH_OBJECT_LAYER_BITS` это **конфигурационные решения** (см. 2 и 3.1). Принять один раз и записать как `inline constexpr` в `Kernel.Config`.

### 1.6 Шапка портированного файла и пример

Комментарий-источник обязателен, формат как в уже сделанных файлах: `(перенесён из Jolt, Core/Reference.h)`. Добавлять версию и отличия:

```cpp
// Интрузивный счётчик ссылок (перенесён из Jolt 5.7.0, Core/Reference.h).
// Отличия от оригинала: счётчик на KernelSync::Atomic, проверки через KernelAssert::DebugCheck.
```

Реальный пример перевода (деструктор `RefTarget`, у вас уже сделан в `Memory.Reference.hpp`; с новым правилом 1.2 тип пишется как `UInt32`).

Jolt (`Core/Reference.h`):
```cpp
inline ~RefTarget()
{
    JPH_IF_ENABLE_ASSERTS(uint32 value = mRefCount.load(memory_order_relaxed);)
    JPH_ASSERT(value == 0 || value == cEmbedded); ///< assert no one is referencing us
}
```

Empire:
```cpp
~RefTarget()
{
    // На объект уже никто не должен ссылаться
    if constexpr (KernelCompiler::IsDebugBuild)
    {
        UInt32 Value = RefCount.Load(KernelSync::MemoryOrder::Relaxed);
        KernelAssert::DebugCheck(Value == 0 || Value == Embedded, "RefTarget: уничтожение объекта, на который ещё ссылаются");
    }
}
```

Что здесь видно: `mRefCount` -> `RefCount`, `cEmbedded` -> `Embedded`, `.load(...)` -> `.Load(...)`, `uint32` -> `UInt32`, макросы `JPH_IF_ENABLE_ASSERTS` и `JPH_ASSERT` ->
`if constexpr` и `DebugCheck` с русским сообщением вида `Класс: что случилось`, английский doc-комментарий -> русский.

### 1.7 Порядок действий для одного файла (чеклист агента)

1. Открыть оригинал Jolt и **все файлы, которые он подключает**. Проверить, что они уже перенесены (по таблицам в разд. 2-4). Нет: сначала переносить зависимости.
2. Выбрать модуль и имя файла (1.2). Положить рядом с похожими файлами, 3D-код Jolt в `*.3D` (1.12). Если модуля нет, создать папку, `xmake.lua`, добавить в `includes`.
3. Если у файла есть движковый аналог, решить по 1.12: заменить или оставить.
4. Переписать файл по таблицам 1.4 и 1.10. Не менять алгоритмы. Не добавлять `std`. Не оставлять ни одного `#define`, `#if`, `JPH_*`, C-приведения (1.11).
5. Проверить на использование методов контейнеров/типов, которых ещё нет в Kernel. Если нет, дописать в Kernel (и в тест Kernel).
6. Собрать gcc и clang, без warning (`-Werror`). Debug и Release.
7. Перенести соответствующие тесты Jolt (1.8).
8. Прогнать под ASan, UBSan, TSan (как для остального Kernel, по README ядра), для многопоточного кода обязательно TSan.
9. Проверить поиском по файлу: нет `#define`, `#if`, `#ifdef`, `JPH`, `typedef`, `memcpy`, `NULL`, `(T)x`, `_API`. В Math/Physics/Engine нет и `#include <...>` системных.
10. Обновить этот файл: поставить `[x]`, добавить строку «где лежит» и список отличий от оригинала.
11. Написать в шапке файла источник и отличия (1.6).

### 1.8 Тесты

- В архиве Jolt есть `UnitTests/` (doctest, папки `Core`, `Math`, `Geometry`, `ObjectStream`, `Physics`, `Compute`). Это эталон: переносить тесты вместе с кодом.
- Тесты ядра: `Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.<Тема>.cpp/.hpp` с функцией `int Run<Тема>UnitTests();` (возвращает 0, если всё прошло), по образцу `Unit.Algorithms.hpp`.
  Подключить в `Main.cpp` и `xmake.lua` (`add_deps` на новый модуль).
- Тесты математики: `Empire.Tests/Tests.Math/Math.Unit` (папка есть). При замене движковых типов Jolt-типами существующие тесты `Math.Unit` это эталон миграции: они должны проходить на новых типах
  либо меняться осознанно (с записью в списке отличий, если изменилось соглашение, например порядок углов или раскладка матрицы).
- Тесты физики: создать `Empire.Tests/Tests.Physics/` по тому же образцу. `PhysicsTestContext` из Jolt (`UnitTests/PhysicsTestContext.*`) переносить как тестовый хелпер.
- Doctest в Empire не использовать (это `std`-тяжёлая библиотека): тесты Jolt переписать на ваш `Run...UnitTests` с проверками через ваш механизм.
- Детерминизм: добавить тест, который гоняет симуляцию N шагов и сравнивает хэш состояния. Золотой хэш хранить в репозитории.

### 1.9 Чего агенту делать нельзя

- Писать или оставлять `#define`, `#if`/`#ifdef`, `JPH_*`, экспортные макросы (`*_API`, `*_EXPORT`). Единственное место для препроцессора: `Kernel.Compiler`.
- Писать код в C-стиле (1.11).
- Подключать `<vector>`, `<string>`, `<atomic>`, `<mutex>`, `<cmath>` и т.п. в Kernel (и в порт Math/физики), а также `Jolt/Jolt.h`.
- Менять формулы, порядок операций, константы, «улучшать» код Jolt.
- Класть горячий код в `.cpp`, если в Jolt он `inline`.
- Переносить файлы без зависимостей и без теста.
- Оставлять рядом два аналога (движковый и Jolt) и писать конвертации между ними: выбрать один по 1.12.
- Сливать несколько файлов Jolt в один и сваливать 3D-код в общие папки: один тип один файл, как в Jolt, 3D в `*.3D`.
- Переименовывать публичные понятия Jolt (`Body`, `Shape`, `BroadPhase`, `Constraint`, `CollideShape`, `NarrowPhaseQuery`), чтобы по документации Jolt можно было искать аналоги. Меняется стиль имён, а не термины.

### 1.10 Макросы Jolt: чем заменяется каждый

Правило: макрос превращается в язык, а не в другой макрос. Таблица покрывает макросы, которые встречаются в Jolt; если найден ещё один, действовать по тому же принципу и дописать строку сюда.

| Макрос Jolt | Что вместо него в Empire |
|---|---|
| `JPH_NAMESPACE_BEGIN/END` | вложенные namespace (1.2) |
| `JPH_ASSERT(x)`, `JPH_ASSERT(x, "msg")` | `KernelAssert::DebugCheck(x, "Класс::Метод: сообщение")` (только Debug); для проверок в Release `KernelAssert::Check`. Место вызова (файл, строка, функция) подставляется аргументами по умолчанию, как в `Compiler.hpp`. Нюанс с вычислением условия: 1.5 |
| `JPH_IF_ENABLE_ASSERTS(...)` | `if constexpr (KernelCompiler::IsDebugBuild) { ... }` |
| `JPH_CRASH`, `JPH_BREAKPOINT` | функция в `Kernel.Compiler` (там уже есть обёртка над `__builtin_trap`) |
| `JPH_EXPORT`, `JPH_EXPORT_GCC_BUG_WORKAROUND` | ничего: видимость задаёт xmake (1.3) |
| `JPH_INLINE`, `inline` | `inline`. Принудительный inline это атрибут `[[gnu::always_inline]]`, он пишется только в обёртках `Kernel.Compiler`; в остальном коде компилятор решает сам |
| `JPH_NODISCARD`, `JPH_FALLTHROUGH` | `[[nodiscard]]`, `[[fallthrough]]` (стандартные атрибуты, не макросы) |
| `JPH_OVERRIDE_NEW_DELETE` | убрать из классов. Память: глобальные `operator new/delete` заменяются один раз в `Kernel.Memory` (отдельный `.cpp`), чтобы `new` и контейнеры шли в аллокатор Empire. Альтернатива: явные `KernelMemory::Make/Destroy`. **Решение, см. 4.5** |
| `JPH_PROFILE(name)`, `JPH_PROFILE_FUNCTION`, `JPH_PROFILE_THREAD_START/END` | RAII-объект `KernelProfiler::Scope Scope("Имя");`; имя функции и строка через аргументы по умолчанию. До появления `Kernel.Profiler` не писать ничего и оставить `// TODO Profiler` |
| `JPH_DECLARE_RTTI_*`, `JPH_IMPLEMENT_RTTI_*`, `JPH_ADD_BASE_CLASS`, `JPH_ADD_ATTRIBUTE`, `JPH_DECLARE_SERIALIZABLE_*` | регистрация шаблонами в `Kernel.Reflection` (имя, базовый класс, поля через указатели на члены, фабрика). Форма API решается при переносе `RTTI.h` (2.2, п.6). Макросов нет |
| `JPH_MAKE_HASHABLE`, `JPH_MAKE_HASH_STRUCT` | специализация шаблона хэша из `Containers.Hash.hpp` для типа |
| `JPH_SUPPRESS_WARNING(S)`, `JPH_CLANG/GCC/MSVC_SUPPRESS_WARNING(S)` | не подавлять: исправить причину. Если исправить нельзя, флаг `-Wno-...` на конкретный target в `xmake.lua` с комментарием, почему |
| `JPH_CPU_X86`, `JPH_CPU_ARM`, `JPH_USE_SSE4_1`, `JPH_USE_AVX2`, `JPH_USE_FMADD`, `JPH_USE_NEON`, `JPH_CPU_ADDRESS_BITS`, `JPH_CPU_*` | `inline constexpr Bool` в `Kernel.Compiler` (`HasSse41`, `HasAvx2`, `HasFma`, `HasNeon`, ...); выбор пути через `if constexpr`. Читаются из препроцессора только там |
| `JPH_VECTOR_ALIGNMENT`, `JPH_DVECTOR_ALIGNMENT` | `inline constexpr UInt32` в `Kernel.Compiler`; в коде `alignas(VectorAlignment)` |
| `JPH_DOUBLE_PRECISION`, `JPH_CROSS_PLATFORM_DETERMINISTIC`, `JPH_OBJECT_LAYER_BITS`, `JPH_ENABLE_ASSERTS`, `JPH_DEBUG_RENDERER`, `JPH_PROFILE_ENABLED` | `inline constexpr` в `Kernel.Config`. Типы от них: условный тип (`Real` это `Double` или `Float` по `Config::DoublePrecision`; шаблон условного типа дописать в `Kernel.Types.Utility`, если его нет) |
| `JPH_PLATFORM_*`, `JPH_COMPILER_*` | не нужны: платформу выбирает xmake набором файлов (1.3, п.4); особенности компилятора только в `Kernel.Compiler` |
| `JPH_OBJECT_STREAM` и прочие «включить фичу» | `inline constexpr Bool` в `Kernel.Config` либо вообще не переносить фичу (4.5) |

### 1.11 C-стиль: что запрещено

| Запрещено | Вместо |
|---|---|
| `#define` как константа | `inline constexpr` |
| `#define` как функция | `inline`, `constexpr`, шаблон |
| `typedef` | `using` (сейчас остался в `Compiler.hpp`, `typedef Vector16`: заменить на `using`) |
| `enum` без класса | `enum class` с указанием базового типа |
| приведение `(T)x`, `T(x)` для преобразований | `static_cast`, `reinterpret_cast` (где без него нельзя), `BitCast` для перепаковки битов |
| `union` для перепаковки битов float/int | `BitCast` (в `Kernel.Types`, дописать, если нет) |
| `memcpy`, `memset`, `memcmp`, `strlen` | `KernelMemory::CopyBytes/FillBytes/CompareBytes`, строковые функции Kernel |
| сырые массивы `T[N]` | `KernelContainers::StaticArray<T, N>` |
| пара «указатель + размер» | типизированный вид на память (span), дописать в `Kernel.Containers`, если нет |
| `void*`, арифметика по `char*` | типизированные указатели и шаблоны; `void*` допустим только в `Kernel.Memory` |
| `NULL`, `0` как указатель | `nullptr` |
| выходные параметры через указатель | ссылка или возвращаемая структура |
| глобальные `static`-переменные и свободные `static`-функции | функция в namespace, `inline constexpr`; `static`-метод класса |
| колбэк `void(*)(void*)` + `void* userData` | `KernelContainers::Function<>` или шаблонный параметр |
| `malloc/free`, голые `new/delete` в логике | RAII: `Ref`, контейнеры, `KernelMemory` |
| `#ifdef`/`#if` ветвления | `if constexpr` по `constexpr`-флагу или файл бэкенда |
| венгерские префиксы (`m`, `c`, `s`, `in`, `out`) | PascalCase без префиксов (1.4) |

### 1.12 Когда Jolt лучше движкового: замена и размещение

1. **Решение по каждому файлу.** Сравнить файл Jolt с движковым аналогом: производительность и SIMD, проверенность и детерминизм, полнота API. Если Jolt лучше, он **заменяет** движковый целиком.
   Движковый файл удаляется, а не остаётся «на всякий случай». Если движковый лучше или у Jolt аналога нет, остаётся движковый (кандидаты в 3.1).
2. **Что есть в движковом и нет в Jolt, сохраняется.** Нужные проектам методы движкового типа (например `Matrix4::Perspective/Orthographic`, `DepthRange`) дописываются к новому типу, а не теряются.
3. **Имена Empire сохраняются, содержимое берётся из Jolt.** Публичные имена в движке остаются (`Vector3`, `Matrix4`, `Quaternion`), чтобы не ломать потребителей; API меняется на Jolt-овский.
   Потребители переводятся на новый API в том же изменении (3.5). Раскладку памяти и соглашения (порядок углов, строки/столбцы) сверить до замены и записать в список отличий.
4. **Размещение по размерности.** Часть Jolt, относящаяся к 3D, идёт в подпапки `*.3D`; часть, относящаяся к 2D (`ConvexHullBuilder2D`, `Float2`, ...), в `*.2D`;
   то, что не зависит от размерности, в общий модуль слоя. Структуру 2D/3D, которая уже есть в движке, не схлопывать. Весь код физики Jolt это `*.3D` (4.1).
5. **Одна модель типов.** Физика и остальной движок работают на одних и тех же математических типах, переходов между «типами Jolt» и «типами движка» нет. Адаптер к ECS/Node это отдельный слой (4.4), но он не конвертирует математику.
6. **Размещение не портит структуру.** Один тип один файл (как в Jolt), файлы кладутся рядом с похожими по смыслу, а не в одну папку.

------------------------------------------------------------------------------------------------------------------------

## 2. ЯДРО: `Jolt/Core` -> `Empire.Kernel`

Принятые решения, записанные как `inline constexpr` значения в `Kernel.Config` (а флаги CPU и компилятора в `Kernel.Compiler`), не как макросы (см. `Core.h`, 1.10):
- кроссплатформенный детерминизм: выключен (`JPH_CROSS_PLATFORM_DETERMINISTIC = false`); не задавать `-ffast-math` или `-march`;
- FMA: запрещён численной политикой 1.1.6 (`UseFMA = false`); не допускать неявного свёртывания операций (`-ffp-contract=off` для GCC/Clang, `/fp:strict` для MSVC);
- двойная точность мира (`JPH_DOUBLE_PRECISION`): влияет на `Real`, `RVec3`, `RMat44` (см. раздел 3);
- битность ObjectLayer (`JPH_OBJECT_LAYER_BITS` 16 или 32);
- нужны ли `JPH_ENABLE_ASSERTS`-аналоги в Release (по умолчанию нет).

В корневом `make.lua` правило `Empire.Module` задаёт `-ffp-contract=off` для GCC/Clang и `/fp:strict` для MSVC. Флаги `-ffast-math` и `-march` не задаются.

### 2.1 Таблица: файл Jolt -> куда в Empire -> статус

| Файл Jolt (`Jolt/Core/`) | Строк | Куда в Empire | Статус |
|---|---|---|---|
| `Array.h` | 697 | `Kernel.Containers/Containers.Array.hpp`, `Containers.FastArray.hpp` | `[x]` (основной API и дополнительные range операции перенесены; исправлены lifetime и пустые итераторы, тесты добавлены) |
| `StaticArray.h` | 325 | `Kernel.Containers/Containers.StaticArray.hpp` | `[x]` |
| `HashTable.h` | 876 | `Containers.FastHashTable.hpp` (+ `FastHashMap`, `FastHashSet`) | `[x]` |
| `UnorderedMap.h`, `UnorderedMapFwd.h`, `UnorderedSet.h`, `UnorderedSetFwd.h` | 160 | `Containers.FastHashMap.hpp`, `FastHashSet.hpp` (+ собственный HashMap) | `[x]` (основной API и пользовательские Hash/KeyEqual проверены; PascalCase и Pair.First/Second — стиль Empire) |
| `HashCombine.h` | 234 | `Containers.Hash.hpp` | `[x]` (`JPH_MAKE_HASHABLE`/`JPH_MAKE_HASH_STRUCT` при переносе файлов-потребителей заменять специализацией хэша, 1.10) |
| `BinaryHeap.h` | 96 | `Kernel.Algorithms/Algorithms.BinaryHeap.hpp` | `[x]` |
| `InsertionSort.h` | 58 | `Algorithms.InsertionSort.hpp` | `[x]` |
| `QuickSort.h` | 137 | `Algorithms.QuickSort.hpp` | `[x]` |
| `FixedSizeFreeList.h`, `.inl` | 349 | `Containers.FixedSizeFreeList.hpp` | `[x]` |
| `LockFreeHashMap.h`, `.inl` | 558 | `Containers.LockFreeHashMap.hpp` | `[x]` (имена Jolt `GetNumBuckets`, `GetMaxBuckets`, `SetNumBuckets`, `GetNumKeyValues`; `GetStatistics` вместо глобального TraceStats) |
| `Memory.h`, `Memory.cpp` | 171 | `Kernel.Memory/Memory.hpp`, `Memory.cpp` | `[x]` (`JPH_OVERRIDE_NEW_DELETE` не переносится: глобальные `operator new/delete` или явный `Make/Destroy`, решение 4.5 п.7) |
| `Reference.h` | 247 | `Memory.Reference.hpp` | `[x]` |
| `TempAllocator.h` | 209 | `Memory.TempAllocator.hpp` | `[x]` (включая `NeedsAlignedAllocate` как constexpr-признак относительно DefaultAlignment; явное выравнивание всегда передаётся KernelMemory) |
| `NonCopyable.h` | 18 | `Kernel.Types/Types.NonCopyable.hpp` | `[x]` |
| `ScopeExit.h` | 49 | `Types.ScopeExit.hpp` | `[x]` |
| `Atomics.h` | 44 | `Kernel.Sync/Sync.Atomic.hpp` | `[x]` (`AtomicMin`/`AtomicMax` добавлены 2026-11-20, покрыты тестами Threading) |
| `JobSystem.h`, `.inl` | 377 | `Kernel.Jobs/Jobs.JobSystem.hpp` | `[x]` (Job сохраняет имя/цвет и оборачивает тело в KernelProfiler::Scope) |
| `JobSystemSingleThreaded.h/.cpp` | 134 | `Jobs.JobSystemSingleThreaded.hpp/.cpp` | `[x]` |
| `JobSystemWithBarrier.h/.cpp` | 343 | `Jobs.JobSystemWithBarrier.hpp/.cpp` | `[x]` |
| `JobSystemThreadPool.h/.cpp` | 506 | `Jobs.JobSystemThreadPool.hpp/.cpp` | `[x]` (пул и Semaphore реализованы; FPExceptionsEnable подключается по Kernel.Config; добавлен точный alias-метод `SetNumThreads` к `SetThreadCount`) |
| `IssueReporting.h/.cpp` | 62 | `Kernel.Assert`, `Kernel.System` | `[x]` (Trace/AssertFailed callbacks добавлены; assertion callback получает expression/message/SourceLocation и управляет breakpoint; Trace использует типобезопасную готовую строку) |
| `Mutex.h` | 225 | `Kernel.Sync/Sync.Mutex.hpp` | `[x]` (Mutex, SharedMutex и RAII lock wrappers адаптированы) |
| `Color.h/.cpp` | 138 | `Empire.Math/Math.Color` | `[x]` (RGBA 8-bit, packed UInt32, channel access, color multiplication/intensity, `ToVec4`, gradients, predefined and distinct colors) |
| `Core.h` | 728 | `Kernel.Compiler/Compiler.hpp`, `Kernel.Config/Config.hpp` | `[x]` (CPU flags, alignment, address bits, precision/determinism, FMA disabled by policy, object-layer width, assertion/FP/profile/debug-renderer/allocator/ObjectStream options and packed compatibility version ID are constexpr; platform/compiler/ISA/sanitizer detection, breakpoint/trap and compiler intrinsics stay inside Compiler) |
| **FP-управление** | | | |
| `FPControlWord.h` | 143 | `Kernel.Compiler/Compiler.hpp` | `[x]` (RAII control word) |
| `FPFlushDenormals.h` | 43 | `Kernel.Compiler/Compiler.hpp` | `[x]` (FTZ/FZ) |
| `FPException.h` | 96 | `Kernel.Compiler/Compiler.hpp` | `[x]` (enable/disable masks; JobSystemThreadPool подключает FPExceptionsEnable по Config) |
| **Обёртки SIMD** | | | |
| Интринсики SSE2/SSE4.1/AVX/AVX2/FMA и скалярный путь (из `Vec3.inl`, `Vec4.inl`, `UVec4.inl`, `Mat44.inl` и др.) | | `Kernel.Compiler/Compiler.Simd.*.hpp` | `[~]` (`Float4`/`UInt4` wrappers expose inline SSE2/NEON/scalar operations; Vector3/4 basic arithmetic, Vector4 `Dot` and UVector4 add/subtract use them; remaining operations/types, AVX paths and full numerical parity remain) |
| `ARMNeon.h` | 340 | `Kernel.Compiler/Compiler.Simd.Neon.hpp` | `[x]` (constructors, generic/specialized shuffle и compile/runtime check source перенесены; отдельная проверка на ARM64 не требуется по решению пользователя) |
| `RISCVVector.h` | 111 | `Kernel.Compiler/Compiler.Simd.RISCV.hpp` | `[x]` (shuffle/reduction templates и compile/runtime check source перенесены; отдельная проверка на RISC-V не требуется по решению пользователя) |
| **Синхронизация** | | | |
| `Semaphore.h/.cpp` | 190 | `Kernel.Threading/Threading.hpp` + платформенные бэкенды | `[x]` (перенесён и адаптирован в Kernel.Threading) |
| `Mutex.h` (`SharedMutex`, `UniqueLock`, `SharedLock`) | | `Sync.Mutex.hpp` | `[x]` (адаптированные RW lock и RAII locks) |
| `MutexArray.h` | 100 | `Sync.MutexArray.hpp` | `[x]` (KernelMemory, GetNumMutexes; profiling macro не привязан из-за зависимости Kernel.Sync -> Kernel.Profiler -> Kernel.Sync) |
| **Сериализация и типы** | | | |
| `Result.h` | 174 | `Kernel.Containers/Containers.Result.hpp` | `[x]` (реализован и покрыт тестами) |
| `StreamIn.h` | 120 | `Kernel.Streams/Streams.StreamIn.hpp` | `[x]` (примитивы, массивы, callback-чтение и Kernel String; short-read/EOF/IO failure обрабатываются; Math-only overloads остаются в Math) |
| `StreamOut.h` | 97 | `Streams.StreamOut.hpp` | `[x]` (примитивы, массивы, callback-запись и Kernel String; Math-only overloads остаются в Math) |
| `StreamUtils.h` | 163 | `Streams.StreamUtils.hpp` | `[x]` (object references, null/shared IDs, single-object type-hash save/restore, typed dispatch callbacks and generic object arrays; callbacks replace global Factory singleton) |
| `StreamWrapper.h` | 57 | `Streams.StreamWrapper.hpp` | `[x]` |
| `ByteBuffer.h` | 74 | `Streams.ByteBuffer.hpp` | `[x]` (добавлены `AppendVector`, byte indexing, iterators, Reserve/PushBack/PopBack/Front/Back, Clear/IsEmpty; Get проверяет границы) |
| `RTTI.h/.cpp` | 616 | `Kernel.Reflection/Reflection.TypeRegistry.hpp/.cpp` | `[x]` (type name/hash/size/alignment, abstract marker, base metadata, IsKindOf, multiple-inheritance typed casts and stable registry metadata; explicit template registration replaces RTTI macros) |
| `Factory.h/.cpp` | 146 | `Reflection.Factory.hpp` | `[x]` (explicit typed/abstract registration, create/find by name/hash/native type, class enumeration and clear; move-only owning Object replaces raw void* lifetime) |
| `ObjectToIDMap.h` | 21 | `Containers.ObjectToIdMap.hpp` | `[x]` (оба alias-имени `IdToObjectMap` и точное `IDToObjectMap`) |
| **Утилиты** | | | |
| `Profiler.h/.cpp/.inl` | 1069 | `Kernel.Profiler` | `[x]` (RAII Scope, callbacks, per-thread 65536-sample buffers, automatic thread registration/lifetime, thread names/IDs, per-name sorted statistics, depth/color inheritance, tick-rate calibration and self-contained HTML timeline/report via BuildHtmlReport) |
| `TickCounter.h/.cpp` | 81 | `Kernel.Compiler` + `Kernel.Time` | `[x]` (x86 RDTSC and ARM64 generic timer through Compiler; Windows non-x86 uses QueryPerformanceCounter in the Windows backend) |
| `StringTools.h/.cpp` | 139 | `Kernel.Containers/Containers.String*.hpp` | `[x]` (Replace/Split/Join/ToLowerAscii/NibbleToBinary/Format/ConvertToString; varargs formatting заменён типобезопасным `{}` Format; Split preserves trailing empty element; Join matches Jolt empty-output delimiter behavior) |
| `StridedPtr.h` | 63 | `Kernel.Containers/Containers.StridedPtr.hpp` | `[x]` (добавлен точный alias `GetPtr` к `GetPointer`) |
| `LinearCurve.h/.cpp` | 118 | `Kernel.Containers/Containers.LinearCurve.hpp` | `[x]` |
| `Prefetch.h` | 20 | `Kernel.Compiler/Compiler.hpp` | `[x]` (PrefetchRead/PrefetchWrite) |
| **Не переносить** | | | |
| `STLAllocator.h`, `STLAlignedAllocator.h`, `STLTempAllocator.h`, `STLLocalAllocator.h` | 449 | | `[-]` (свои контейнеры без std-аллокаторов) |
| `IncludeWindows.h` | 36 | | `[-]` (Windows-заголовки только в `Platform`/`Kernel.System`) |
| `LSANSuppressions.h`, `TSANSuppressions.h` | 52 | `Empire.Build` / конфиг санитайзеров | `[-]` (по желанию) |

### 2.2 Порядок переноса ядра

1. **[x] Конфигурация, флаги и FP:** решения из начала раздела как `constexpr`, флаги CPU в `Kernel.Compiler`, `Prefetch`, `FPControlWord`, `FPFlushDenormals`, `FPException`, потом убрать TODO в `JobSystemThreadPool.cpp`. В этот же шаг: убрать `*_API`/`_EXPORT` из заглушек и из корневого `xmake.lua` (1.3), заменить `typedef` в `Compiler.hpp` на `using`.
2. **[x] Синхронизация:** `Semaphore` (+ бэкенды), `SharedMutex`/`SharedLock`/`UniqueLock`, `MutexArray`. Проверить `JobSystemThreadPool` на `Semaphore` (оригинал использует его для сна воркеров).
3. **[x] Типы:** `Result`, `StridedPtr`, `ObjectToIDMap`, `Prefetch`, недостающие методы `Array`/`HashMap`.
4. **[x] Time и String:** `TickCounter`, `StringTools`.
5. **[x] Streams:** `StreamIn/Out/Utils/Wrapper`, `ByteBuffer`.
6. **RTTI/Factory** — типизированный реестр, иерархия, abstract/concrete state и создание по имени/hash/native key перенесены.
7. **Profiler** — per-thread buffers, thread metadata, callbacks, statistics и HTML report доступны через Kernel API.
8. **ISA wrappers в Kernel.Compiler:** ARM64 NEON и RISC-V Vector перенесены; дополнительные кросс-платформенные компиляции не являются условием завершения. SSE/AVX/FMA vector math из `Vec*.inl` относится к последующему Math этапу.

### 2.3 Повторная сверка фактического состояния Empire (2026-11-18)

Предыдущая таблица 2.1 отражает состояние на 2026-11-16 и частично устарела. Повторная проверка архива Empire от 2026-11-18 показывает:

- `Semaphore` уже реализован как `Kernel.Threading::Semaphore`; ОС-бэкенды Linux, Windows и macOS уже существуют. В macOS thread backend при создании потока сохраняется QoS-класс вызывающего потока и применяется к новому потоку, как в Jolt JobSystemThreadPool, чтобы избежать priority inversion. Не переносить второй семафор в `Kernel.Sync`.
- `SharedMutex`, `UniqueLock`, `SharedLock` и `LockGuard` уже находятся в `Kernel.Sync/Sync.Mutex.hpp`.
- `MutexArray` уже существует в `Kernel.Sync/Sync.MutexArray.hpp`.
- FP control word, flush-denormals, FP exceptions, prefetch, CPU feature flags и processor tick counter уже представлены в `Kernel.Compiler/Compiler.hpp`; `Kernel.Jobs/Jobs.JobSystemThreadPool.cpp` уже включает `FPExceptionsEnable` по `Kernel.Config::FloatingPointExceptions`.
- `Result`, `StridedPtr`, `ObjectToIdMap`, `LinearCurve`, `StringTools` и все пять Stream/ByteBuffer компонентов уже существуют в соответствующих модулях Kernel.
- `Profiler` уже существует в `Kernel.Profiler`. В этой сверке исправлен учёт глубины вложенности: глубина Scope теперь учитывается и восстанавливается даже если запись не попала в заполненный sample buffer; внешний callback остаётся независимым от внутреннего sampling. `JobSystem::Job::Execute` передаёт имя и цвет задачи в `KernelProfiler::Scope`, а `JobSystemWithBarrier`/`JobSystemThreadPool` получили соответствующие scope-замеры из Jolt.
- `Array` уже содержит `Front`, `Back`, `PopBack`, `Insert`, `Resize` и `Find`; старое замечание о недостающих методах неактуально. `HashMap` также имеет итерацию и `Reserve` (проверено по Kernel unit tests).
- `TypeRegistry` и `Factory` уже реализованы и покрыты тестами, включая множественное наследование и создание по имени/хэшу. Они являются адаптацией Jolt RTTI/Factory, а не отсутствующими файлами. `RegisterBase` дополнительно защищён от циклов и повторной регистрации прямой связи; Factory сериализует регистрацию/создание/очистку общим mutex, чтобы не получить частично зарегистрированный тип при конкурентном доступе.
- `Kernel.Config` дополнен версиями Physics 5.7.0, feature flags и `PhysicsVersionId` по битовой раскладке Jolt `Core/Core.h`; значения являются `constexpr`, препроцессорные feature macros не экспортируются. `Color` относится к `Empire.Math`, но его сверка с Jolt `Core/Color.h/.cpp` выполнена: добавлены RGBA 8-bit хранение, packed UInt32, доступ/изменение каналов, умножение цветов и интенсивности, `ToVec4`, интенсивность, зелёно-красный градиент, палитра и distinct colors. Добавлен отдельный `Math.Unit` test source. NEON-обёртки ARM64 теперь находятся в `Kernel.Compiler`; проверка на ARM64 ожидает, а SIMD-математика `Vector3/4` остаётся в последующем этапе Math.
- `Array` расширен по фактическому публичному API Jolt: добавлены конструкторы по размеру/значению и диапазону, `Resize(count, value)`, reverse iterators, `cbegin/cend`, `data`, `at`, `size/capacity/empty`, range `Insert`/`Assign`/`Erase`, `ShrinkToFit`, `Swap`, сравнение и `GetHash`. Перемещение элементов при `Insert`/`RemoveAt` переведено на move-construction + destruction, чтобы не требовать move-assignment у типа элемента.
- `MutexArray` и массив `BarrierImpl` в `JobSystemWithBarrier` переведены с `new[]/delete[]` на `KernelMemory::Allocate/Deallocate` и `ConstructAt/DestroyAt`, сохраняя нужное выравнивание и корректное разрушение элементов. Проверено поиском: в `Engine.Kernel` больше нет `new[]/delete[]` для массивов. Это соответствует правилу Kernel не использовать глобальные new/delete для собственного хранения. API `MutexArray` дополнен именем `GetNumMutexes`, совпадающим по смыслу с оригиналом Jolt; `GetMutexCount` оставлен как внутреннее имя-синоним для уже существующих потребителей.
- Дополнительно сверены `MutexArray` и `TempAllocator` с исходниками Jolt: `MutexArray` использует локальную реализацию того же FNV-1a по байтам `UInt32`, что и `KernelContainers::Hash<UInt32>`/Jolt, чтобы не создавать циклическую зависимость Kernel.Sync -> Kernel.Containers; `TempAllocator` выбирает выравнивание по конфигурации мира — `VectorAlignment` для `Float` и `DoubleVectorAlignment` для `Double`; переполнение AlignUp и underflow при Free проверяются до изменения стека. Проверка принадлежности памяти в `TempAllocatorImpl::OwnsMemory` выполняется через `UIntPtr`, без сравнения несвязанных указателей. `Kernel.Memory` получил зависимость от `Kernel.Config`.
- `StreamIn`/`StreamOut` дополнены перегрузками сериализации массивов с пользовательским callback на элемент, как в Jolt 5.7.0, а также чтением/записью `KernelContainers::String` в формате `UInt32 length + raw chars`. `StreamUtils::SaveObject`/`RestoreObject` дополнены симметричной записью/чтением type hash через явный callback вместо глобального Factory singleton; `SaveObjectArray`/`RestoreObjectArray` обобщены на контейнеры Kernel (включая `Array` и `FastArray`) вместо жёсткой привязки к одному типу. В `RestoreObjectReference` добавлена отдельная проверка отсутствующего callback и проверка EOF после восстановления, чтобы не возвращать успешный объект при неполном потоке. Добавлены unit cases для custom element callbacks, round-trip строки и сериализации object array между разными контейнерами.
- Добавлены Kernel unit cases для range insert/erase/assign, reverse iteration, shrink/swap/equality/hash и size/value/range constructors. По просьбе пользователя сборку и исправление ошибок компиляции отложить до окончания функционального переноса; эти тесты пока не запускались.

В рамках предыдущих шагов RTTI добавлены `GetBaseClassCount`, `GetBaseClass`, сравнение описаний типов, `IsType`, `IsKindOf` и типизированные `DynamicCast`-помощники. Factory переведена с глобальных `new/delete` на `KernelMemory::Allocate/Deallocate` с корректным освобождением при исключении конструктора. Дополнены Kernel unit tests на метаданные и cross-cast при множественном наследовании. В текущем шаге `RegisterBase` защищён от циклических отношений, а операции Factory по регистрации/созданию/очистке синхронизированы.

Статус проверки: до последнего набора изменений ранее проходила отдельная синтаксическая проверка `Unit.Kernel.cpp` GCC C++20 с `-Wall -Wextra -Werror`. После расширения Array, Streams, MutexArray, JobSystemWithBarrier, HashMap/Set, RTTI/Factory, Profiler и интеграции профайлера во все соответствующие JobSystem paths сборка/синтаксическая проверка и запуск тестов намеренно отложены по просьбе пользователя до завершения функционального переноса. Полная сборка в этой среде также ограничена отсутствием xmake.

------------------------------------------------------------------------------------------------------------------------

## 3. МАТЕМАТИКА: `Jolt/Math` и `Jolt/Geometry` -> `Empire.Math`

### 3.1 Состояние Empire.Math (проверено) и стратегия: Jolt заменяет, а не живёт рядом

Старая реализация неперенесённых Math-модулей удалена, чтобы не смешивать её с переносом Jolt; структура каталогов и xmake-манифесты сохранены. Идут Jolt-порты scalar, Vector, Matrix, Quaternion и Geometry. В переходных vector/matrix файлах ещё могут оставаться старые совместимые API; они будут устранены при завершении замены типов. `Vector2`, Transform, прежняя custom Geometry.2D, Random и SIMD-заглушки очищены; `ConvexHullBuilder2D` уже восстановлен из Jolt, остальные пункты продолжаются. Текущие scalar-реализации не являются завершением: полный API diff, SIMD и платформенная/численная сверка ещё требуются.

Модель Jolt принципиально другая: `Vec3`, `Vec4`, `UVec4`, `BVec16`, `Mat44`, `Quat` хранятся в 128-битных SIMD-регистрах (`Vec3` это 4 lane, 16 байт, последняя lane мусор/дубль). `Float3` это тип хранения (12 байт).

**Стратегия (решено): Jolt-математика заменяет движковую, параллельных типов и конвертаций нет.**
1. Jolt-реализациями заменяются: `Vector3`, `Vector4`, `Matrix4`, `Quaternion`, `AABB`, `OBB`, `Plane`, `Sphere` (геометрия в 3.3). Типы без аналога Jolt (`Vector2`, `Matrix2/3`, `Frustum`, `Bezier`, `CatmullRom`, `Geometry.2D`, `Random`, `Transform`) остаются частью целевой структуры, но прежняя реализация удалена по просьбе пользователя и будет восстановлена отдельно при необходимости. `Color` сверяется с Jolt `Color` (2.1).
2. Имена типов Jolt получают имена Empire без сокращений (публичные имена в движке сохраняются):

   | Jolt | Empire | | Jolt | Empire |
   |---|---|---|---|---|
   | `Vec3` | `Vector3` | | `Mat44` | `Matrix4` |
   | `Vec4` | `Vector4` | | `Quat` | `Quaternion` |
   | `UVec4` | `UVector4` | | `DVec3` | `DVector3` |
   | `BVec16` | `BVector16` | | `DMat44` | `DMatrix4` |
   | `Float2/3/4`, `Double3` | `Float2/3/4`, `Double3` (типы хранения) | | `RVec3`, `RMat44` | `RVector3`, `RMatrix4` |

   Имена на согласование при переносе; главное, чтобы они были одинаковыми во всём коде.
3. `Vector3` больше не структура с полями `X/Y/Z`: доступ через `GetX()/SetX()`, как в Jolt. Это ломает потребителей, их переводят в том же изменении (3.5).
   Всё, что было у движковых типов и нужно проектам, а в Jolt нет (`Matrix4::Perspective/Orthographic`, `DepthRange`, и т.п.), дописывается к новому типу (1.12, п.2). Сверка по файлам Jolt обязательна, список не гадать.
4. **SIMD без препроцессора.** Проверки ISA и интринсики живут только в `Kernel.Compiler`:
   - `Compiler.Cpu.hpp`: `constexpr`-флаги `HasSse2/HasSse41/HasAvx/HasAvx2/HasFma/HasNeon`;
   - `Compiler.Simd.*.hpp`: тонкие обёртки регистра и примитивных операций над интринсиками SSE2/SSE4.1/AVX/AVX2/NEON/скалярный путь, с `[[gnu::always_inline]]`.
   Math строит `Vector3/4`, `UVector4`, `BVector16`, `Matrix4`, `Quaternion` поверх этих обёрток через `if constexpr`. Прецедент уже есть: `EqualMask16` в `Compiler.hpp` использует `__builtin_ia32_pmovmskb128`.
   Заглушки `Math.Simd/Simd.SSE|AVX|NEON|Scalar` (отдельные `.so` без кода) удаляются; `Math.Simd` остаётся под то, что не зависит от ISA (`Swizzle`, маски), или тоже удаляется, решить при переносе.
   **Риск, проверить прототипом до массового переноса:** ветка `if constexpr` с интринсиком ISA, которой нет в текущей сборке, может не компилироваться на gcc или clang (проверить на трёх операциях: сложение, dot, blend).
   Если не компилируется, выбор бэкенда делает xmake отдельными файлами по архитектуре (как `Kernel.System/System.Linux`), не препроцессор.
5. Порядок и выбор путей Jolt (SSE2, SSE4.1, AVX, AVX2, FMA, NEON, скалярный) определяет детерминизм, не «упрощать» до одного.
6. Выбрать `Real` (float или double) один раз как `inline constexpr` в `Kernel.Config` и `using Real` от него (`Real.h`, `MathTypes.h`).

### 3.2 Таблица: `Jolt/Math` -> Empire

| Файл Jolt (`Jolt/Math/`) | Строк | Куда в Empire и что заменяет | Статус |
|---|---|---|---|
| `Math.h` (Clamp, Square, Cubed, Sign, битовые операции, степени двойки, выравнивание) | 278 | `Math.Scalar/Scalar.hpp` (+ часть в `Kernel.Types.Utility`, где уже есть Count*-подобные) | `[x]` (публичные scalar helpers сверены с Jolt; сохранена семантика min/max/clamp, включая равные значения и signed zero; граничные случаи степеней двойки покрыты тестами) |
| `Trigonometry.h` | 79 | `Math.Vector/Trigonometry.hpp` (scalar wrappers in `MathScalar`, kept in the Vector target because they depend on `Vector4`) | `[x]` (все scalar wrappers перенесены поверх детерминированных Vec4 polynomial operations и покрыты тестами) |
| `Real.h`, `MathTypes.h` | 76 | `Math.Scalar/MathTypes.hpp` (`Real` от `Config`, world/vector argument aliases без макросов) | `[~]` (`Real`, storage/world aliases и argument aliases добавлены; `DVector3`/`DMatrix4` остаются forward-declared до double-precision этапа) |
| `Swizzle.h` | 19 | `Math.Vector` (или `Math.Simd`) | `[~]` (scoped component enum added; SIMD swizzle API still being ported) |
| `Float2.h` | | `Math.Vector/Float2.hpp` (2D); сверить с `Vector2`: если Jolt лучше, заменяет | `[~]` (тип хранения портирован; прежний Vector2 удалён до Jolt-переноса) |
| `Float3.h`, `Float4.h`, `Double3.h` | 187 (все четыре) | `Math.Vector/Float3.hpp`, `Float4.hpp`, `Double3.hpp` (типы хранения, 12 / 16 / 24 байта) | `[~]` (`Float3`/`Double3` hash support, `VertexList` alias and trivial-copy/layout checks added; full source comparison remains) |
| `HalfFloat.h` | 208 | `Math.Scalar/HalfFloat.hpp` and `Math.Vector/HalfFloatConversion.hpp` | `[~]` (scalar rounding modes and four-lane unpack conversion ported and checked across all 65536 half patterns; SIMD conversion backend pending) |
| `Vec3.h/.inl/.cpp` | 1659 | `Math.Vector/Vector3.hpp`, **заменяет** движковый `Vector3` | `[~]` (API includes Jolt-order random/spherical vectors, the 1026-point unit sphere, precise cross product and unit-vector compression; basic arithmetic now uses SSE2/NEON/scalar `Float4`; remaining SIMD operations and full source comparison remain) |
| `Vec4.h/.inl` | 1783 | `Math.Vector/Vector4.hpp`, **заменяет** движковый `Vector4` | `[~]` (scalar API includes comparisons, masks, gather, reductions, compression, deterministic trig and the original three-pass sort network including duplicate ordering; SSE2/NEON/scalar arithmetic wrapper is now used by inline add/subtract/multiply/`Dot`; remaining SIMD operations, full source comparison and consumer migration remain) |
| `UVec4.h/.inl` | 1133 | `Math.Vector/UVector4.hpp` | `[~]` (public API includes masks, gather, shifts/unpack; inline add/subtract now use SSE2/NEON/scalar `UInt4`; other SIMD operations and full source comparison remain) |
| `BVec16.h/.inl` | 276 | `Math.Vector/BVector16.hpp` | `[~]` (portable byte implementation covers construction, masks, logical ops and truth queries; SIMD backend and full API comparison remain) |
| `Mat44.h/.inl` | 1419 | `Math.Matrix/Matrix4.hpp`, **заменяет** движковый `Matrix4` (+ допись `Perspective/Orthographic/DepthRange`) | `[~]` (scalar API, inverse, decomposition, transforms и quaternion interop реализованы; исправлены `GetRotationSafe`, scaling и 3x3 inverse semantics; SIMD, полный API diff и численная сверка остаются) |
| `Quat.h/.inl` | 731 | `Math.Quaternion/Quaternion.hpp`, **заменяет** движковый `Quaternion` | `[~]` (scalar quaternion API и Matrix4 interop реализованы; random quaternion sequence preserves Jolt's paired SinCos evaluation and is bit-checked; полная сверка всех ветвей и SIMD/API дополнения остаются) |
| `DVec3.h/.inl` | 1566 | `Math.Vector/Vector.DVector3.hpp` | `[ ]` (нужен только при двойной точности) |
| `DMat44.h/.inl` | 565 | `Math.Matrix/DMatrix4.hpp` | `[ ]` (то же) |
| `Vector.h`, `Matrix.h`, `DynMatrix.h` | 507 | `Math.Matrix/{Vector,Matrix,DynMatrix}.hpp` | `[~]` (fixed-size templates and dynamic storage ported; ostream omitted per Empire no-system-headers rule; Jolt matrix tests ported) |
| `EigenValueSymmetric.h` | 177 | `Math.Matrix/EigenValueSymmetric.hpp` | `[~]` (Jacobi solver and symmetric 3x3 eigenpair test ported; broader randomized comparison pending) |
| `GaussianElimination.h` | 102 | `Math.Matrix/GaussianElimination.hpp` | `[x]` (Gauss-Jordan elimination ported; fixed matrix inverse paths covered) |
| `FindRoot.h` | 50 | `Math.Scalar/FindRoot.hpp` | `[x]` (quadratic/linear/constant cases ported and covered by scalar unit tests) |
| `ARMNeon.h` (из Core) | 340 | `Kernel.Compiler/Compiler.Simd.Neon.hpp` (обёртки интринсиков, 3.1 п.4) | `[x]` (Core перенос завершён; отдельная ARM64 verification не входит в критерий по решению пользователя) |

Важно для Vec3/Vec4/Mat44/Quat:
- В Jolt несколько реализаций под SSE2, SSE4.1, AVX, AVX2, FMA, NEON и скалярную. Порядок и выбор путей определяет детерминизм, не «упрощать» до одной.
- Тригонометрия (`Vec4::sSin`, `sCos`, `sATan`, ...) реализована своим кодом именно ради детерминизма. Не подменять `libm`.
- `Vec3::sRandom`, `Vec3::GetNormalizedPerpendicular`, `Quat::sEulerAngles` и другие мелкие функции: переносить все, физика использует.

### 3.3 Таблица: `Jolt/Geometry` -> Empire

Расположение: 3D-алгоритмы идут в `Geometry.3D`, 2D в `Geometry.2D` (1.12, п.4). Чисто геометрические алгоритмы (без понятия «физика») идут в `Math.Geometry`. Алгоритмы, работающие
поверх интерфейса «поддержка выпуклой формы» (GJK, EPA, ConvexSupport) идут в `Engine.Physics/Physics.Collision/Collision.3D`, так как без шейпов они не используются.
Если хочется быть ближе к Jolt, их можно оставить в `Math.Geometry`. Принять решение один раз.

| Файл Jolt (`Jolt/Geometry/`) | Строк | Куда в Empire | Статус |
|---|---|---|---|
| `AABox.h` | 313 | `Math.Geometry/Geometry.3D/AABB.*`, **заменяет** движковый `AABB` | `[~]` (float API, plane overlap и triangle/indexed-triangle construction/encapsulation перенесены и покрыты тестами; double-precision overloads ждут соответствующих типов) |
| `AABox4.h` | 224 | `Math.Geometry/Geometry.3D/AABB4.hpp` | `[~]` (four-lane overlap/point/sphere/distance/scale/extent and OBB queries ported; current OBB path evaluates lanes scalar, packed SIMD and complete numerical comparison remain) |
| `OrientedBox.h/.cpp` | 217 | `Geometry.3D/OBB.hpp`, **заменяет** движковый `OBB` | `[~]` (SAT overlap для AABB и OBB портирован, identity/rotated cases покрыты; финальный API/численный diff остаётся) |
| `Plane.h` | 104 | `Geometry.3D/Plane.hpp`, **заменяет** движковый `Plane` | `[~]` (float API: point/normal, CCW points, transform/scale, projection и intersection; double overload и полная численная сверка остаются) |
| `Sphere.h` | 72 | `Geometry.3D/Sphere.hpp`, **заменяет** движковый `Sphere` | `[~]` (float API, overlap и point encapsulation перенесены; исходный API diff остаётся) |
| `Triangle.h`, `IndexedTriangle.h` | 164 | `Geometry.3D/{Triangle,IndexedTriangle}.hpp` | `[~]` (storage, centroid, equivalence/opposite/rotation, degeneracy, canonical index ordering, hashing и AABB integration ported; compare complete API/layout) |
| `Ellipse.h` | 77 | `Geometry.3D/Ellipse.hpp` | `[~]` (inside test, closest point and normal ported; compare full API/edge behavior) |
| `ClosestPoint.h` | 494 | `Geometry.3D/ClosestPoint.hpp` | `[~]` (barycentric coordinates and closest point to line/triangle/tetrahedron ported, including degenerate triangles and feature masks; complete numerical/API comparison remains) |
| `ClipPoly.h` | 200 | `Geometry.3D/ClipPoly.hpp` | `[~]` (plane, polygon, edge and AABB clipping ported to Kernel Array-like containers; regression tests cover plane/poly/box, fuller edge-case parity remains) |
| `MortonCode.h` | 40 | `Geometry.3D/MortonCode.hpp` | `[~]` (10-bit expansion and 3D Morton code ported; bounds and edge cases require broader validation) |
| `RayAABox.h`, `RayCapsule.h`, `RayCylinder.h`, `RaySphere.h`, `RayTriangle.h` | 633 | `Geometry.3D/Ray*.hpp` | `[~]` (scalar ray-box, sphere, cylinder, capsule and triangle intersection APIs plus four-lane RayAABox/RayTriangle fallbacks implemented; packed SIMD paths for other queries and complete API/numerical comparison remain) |
| `Indexify.h/.cpp` | 241 | `Geometry.3D/Indexify.hpp` | `[~]` (vertex welding and deindexification ported with stable first-index representatives; optimized spatial partition and broad parity tests remain) |
| `ConvexHullBuilder.h/.cpp`, `ConvexHullBuilder2D.h/.cpp` | 2186 | `ConvexHullBuilder` в `Geometry.3D/ConvexHullBuilder.*`; `ConvexHullBuilder2D` в `Geometry.2D/ConvexHullBuilder.*` | `[x]` (2D conflict-list hull; 3D `ConvexHullBuilder` is a function-by-function port of Jolt 5.7.0: conflict lists, FindEdge, MergeFaces, MergeDegenerateFace, MergeCoplanarOrConcaveFaces, RemoveInvalidEdges, RemoveTwoEdgeFace, Validate*; debug drawing and shape dump are not ported. `Unit.ConvexHull` passes including the noisy stress loop; a 3000-seed randomized stress passes under ASan/UBSan with asserts on, and `DetermineMaxError` on near-planar seeds matches Jolt's value bit for bit. Line-by-line numeric parity and SIMD path remain) |
| `ConvexSupport.h` | 188 | `Physics.Collision/Collision.3D` | `[ ]` |
| `GJKClosestPoint.h` | 950 | `Physics.Collision/Collision.3D` | `[ ]` |
| `EPAConvexHullBuilder.h` | 843 | `Physics.Collision/Collision.3D` | `[ ]` |
| `EPAPenetrationDepth.h` | 557 | `Physics.Collision/Collision.3D` | `[ ]` |

Не из Jolt и остаётся как есть: `Frustum`, `Bezier`, `CatmullRom` и существующая 2D-геометрия (кроме отдельно перенесённого `ConvexHullBuilder2D`).

### 3.4 Порядок переноса математики

1. Решения 3.1 (SIMD-стратегия, `Real`, конфиг), `Math.h`, `Trigonometry.h`, `Real.h`, `MathTypes.h`, `Swizzle.h`.
2. `Float2/3/4`, `HalfFloat` (или отображение на `Vector*`).
3. `Vec4`, `UVec4`, `BVec16`, потом `Vec3` (оно использует `Vec4`), потом `Quat`, `Mat44`. После каждого типа: удалить движковый аналог, перевести потребителей (3.5), прогнать `Tests.Math`.
4. `DVec3`, `DMat44` (если нужна двойная точность).
5. `Vector`, `Matrix`, `DynMatrix`, `GaussianElimination`, `EigenValueSymmetric`, `FindRoot`.
6. Тесты из `UnitTests/Math` и `UnitTests/Geometry` на каждом шаге.
7. Геометрия (3.3) по списку сверху вниз: простые функции, потом `AABox4`, `ConvexHullBuilder`, потом GJK/EPA.

### 3.5 Замена движковых типов: порядок и потребители

Потребители (по подключениям заголовков на момент проверки): `Math.Transform/Transform.3D`, `Math.Geometry/Geometry.3D` (AABB, OBB, Plane, Sphere, Ray, Frustum, Bezier, CatmullRom), тесты `Tests.Math/Math.Unit`.
Для каждого заменяемого типа (`Vector3`, `Vector4`, `Matrix4`, `Quaternion`, затем геометрия):
1. Выписать публичный API движкового типа и пометить, что из него есть в Jolt, а что нет.
2. Перенести тип Jolt по чеклисту 1.7 в файл движкового типа (`Vector3.hpp` и т.д.), дописать недостающее из п.1.
3. Перевести потребителей на новый API (`GetX()` вместо `X` и т.п.) в том же изменении, удалить `.cpp` старой реализации.
4. Прогнать `Math.Unit`: тесты, которые прошли раньше, должны проходить; изменение соглашения (порядок углов `Matrix4::Rotation`, строки/столбцы, диапазон `DepthRange`) это осознанная правка теста с записью в список отличий.
5. Только после этого переходить к следующему типу.

------------------------------------------------------------------------------------------------------------------------

## 4. САМА ФИЗИКА: `Jolt/Physics` и сопутствующее -> `Empire.Engine/Engine.Physics`

### 4.1 Состояние Engine.Physics (проверено)

Есть только скелет подмодулей с пустыми namespace (около 21 строки на заголовок), без реализации:

```
Physics.Shape/      (Shape.2D, Shape.3D)
Physics.Collision/  (Collision.2D, Collision.3D)
Physics.Solver/     (Solver.2D, Solver.3D)
Physics.Constraint/ (Constraint.2D, Constraint.3D)
Physics.Body/       (Body.2D, Body.3D)
Physics.Query/      (Query.2D, Query.3D)
Physics.Character/  (Character.2D, Character.3D)
Physics.Debug/
```
`Engine.Physics/xmake.lua` зависит от `Engine.ECS`, `Engine.Node`, `Math.Geometry`. Эти зависимости **физике не нужны**: порт Jolt должен зависеть только от `Kernel*` и `Math*`.
Связка с ECS/Node это отдельный адаптерный слой (создать модуль, например `Physics.Bridge`), а не часть портируемого кода.

Jolt это **только 3D**. Структура 2D/3D в скелете: переносить в `*.3D`. Для `*.2D` у Jolt аналога нет: либо отдельная собственная реализация, либо 2D как 3D с ограничением степеней
свободы (`AllowedDOFs.h`, режим `Plane2D`). Решить позже, переносить сначала 3D.

Размещение (1.12, п.4): весь портируемый код физики Jolt это 3D и идёт в `*.3D`. Если окажется, что файл Jolt (или его часть) относится к 2D, он идёт в `*.2D`; если не зависит от размерности, в общий модуль подмодуля.
Структуру `2D/3D` скелета не схлопывать и не складывать 3D-код в общие папки «на потом».

Нужные подмодули, которых в скелете нет: `Physics.BroadPhase` (или внутри `Collision`), `Physics.Vehicle`, `Physics.Ragdoll`, `Physics.SoftBody`, `Physics.Skeleton`, `Physics.Serialization`.

### 4.2 Раскладка Jolt -> Empire (карта модулей)

| Папка Jolt | Файлов | Куда в Empire | Статус |
|---|---|---|---|
| `Physics/Body/` | 26 | `Physics.Body/Body.3D` | `[ ]` |
| `Physics/Collision/Shape/` | ~44 | `Physics.Shape/Shape.3D` | `[ ]` |
| `Physics/Collision/` (фильтры, collectors, касты, narrow phase) | 54 | `Physics.Collision/Collision.3D`, запросы в `Physics.Query/Query.3D` | `[ ]` |
| `Physics/Collision/BroadPhase/` | ~13 | `Physics.Collision/Collision.3D` (или новый `Physics.BroadPhase`) | `[ ]` |
| `Physics/Constraints/` | 41 | `Physics.Constraint/Constraint.3D` | `[ ]` |
| корень `Physics/` (`PhysicsSystem`, `IslandBuilder`, `LargeIslandSplitter`, `PhysicsUpdateContext`, ...) | 20 | `Physics.Solver/Solver.3D` (+ верхний `Physics.hpp`) | `[ ]` |
| `Physics/Character/` | 7 | `Physics.Character/Character.3D` | `[ ]` |
| `Renderer/` (`DebugRenderer`, `Draw*`) | 8 | `Physics.Debug` | `[ ]` (зависит от вашей графики, делать в конце) |
| `AABBTree/`, `TriangleSplitter/` | 9 | `Physics.Shape/Shape.3D` (нужны `MeshShape`, `HeightFieldShape`) | `[ ]` |
| `ObjectStream/` | 23 | `Kernel.Reflection` или `Physics.Serialization` | `[ ]` |
| `Physics/Vehicle/` | 24 | `Physics.Vehicle` (нового нет) | `[ ]` (низкий приоритет) |
| `Physics/Ragdoll/`, `Skeleton/` | 2 + 8 | `Physics.Ragdoll`, `Physics.Skeleton` | `[ ]` (низкий приоритет) |
| `Physics/SoftBody/` | 12 | `Physics.SoftBody` | `[ ]` (низкий приоритет) |
| `Physics/Hair/` | 8 | | `[-]` (нужен Compute, пока не нужен) |
| `Compute/`, `Shaders/` | 5 + 49 | | `[-]` (GPU-вычисления Jolt для hair/softbody) |
| `Jolt.h`, `RegisterTypes.*`, `ConfigurationString.h` | | `Physics.hpp` / `Physics.Register.*` | `[ ]` (регистрация типов, будет после RTTI/Factory) |

### 4.3 Подробно: что внутри и в каком порядке

#### Этап A. Данные тела и базовые типы (после ядра и математики)
Файлы: `Body/BodyID.h`, `BodyType.h`, `MotionType.h`, `MotionQuality.h`, `AllowedDOFs.h`, `EActivation.h`, `EPhysicsUpdateError.h`,
`PhysicsSettings.h`, `Collision/ObjectLayer.h`, `Collision/BroadPhase/BroadPhaseLayer.h`, `Body/MassProperties.*`, `Body/MotionProperties.*` (+`.inl`), `Body/Body.*` (+`.inl`),
`Body/BodyCreationSettings.*`, `Collision/PhysicsMaterial*.{h,cpp}`, `Collision/CollisionGroup.*`, `GroupFilter*.{h,cpp}`.
Куда: `Physics.Body/Body.3D`. Тест: `UnitTests/Physics` (создание тела, масса, inertia).

#### Этап B. Шейпы
Файлы `Collision/Shape/`: сначала `Shape.*`, `SubShapeID.h`, `SubShapeIDPair.h`, `ConvexShape.*`, `DecoratedShape.*`, потом выпуклые (`SphereShape`, `BoxShape`, `CapsuleShape`,
`CylinderShape`, `TaperedCapsuleShape`, `TaperedCylinderShape`, `TriangleShape`, `PlaneShape`, `EmptyShape`, `ConvexHullShape`), потом декораторы (`ScaledShape`,
`RotatedTranslatedShape`, `OffsetCenterOfMassShape`), потом составные (`CompoundShape`, `StaticCompoundShape`, `MutableCompoundShape`), в конце `MeshShape`, `HeightFieldShape`
(им нужны `AABBTree`, `TriangleSplitter`, `StridedPtr`).
Зависит от: геометрии (3.3), `Result`, `Streams`, RTTI/Factory (сохранение шейпов).

#### Этап C. Коллизии (narrow phase)
Файлы `Collision/`: `CollisionCollector*.h`, `CollideShape.h`, `RayCast.h`, `ShapeCast.h`, `CastResult.h`, `CollisionDispatch.*`, `ShapeFilter.h`, `ContactListener.h`,
`ManifoldBetweenTwoFaces.*`, `CollideConvexVsTriangles.*`, `CollideSphereVsTriangles.*`, `CastConvexVsTriangles.*`, `CastSphereVsTriangles.*`, `ActiveEdges.h`,
`InternalEdgeRemovingCollector.h`, `EstimateCollisionResponse.*`, `TransformedShape.*`, `NarrowPhaseQuery.*`, `NarrowPhaseStats.*`.
Зависит от: этап B, GJK/EPA/`ConvexSupport`.

#### Этап D. Broad phase
Файлы `Collision/BroadPhase/`: `BroadPhase.*`, `BroadPhaseQuery.h`, `BroadPhaseBruteForce.*`, `BroadPhaseQuadTree.*`, `QuadTree.*`, `BroadPhaseLayerInterface*`, `ObjectVsBroadPhaseLayerFilter*`,
`ObjectLayerPairFilter*`. Зависит от: `AABox4`, `FixedSizeFreeList`/`LockFreeHashMap` (уже есть), `Semaphore`/`SharedMutex`.

#### Этап E. Менеджеры тел и констрейнтов
Файлы: `Body/BodyManager.*`, `Body/BodyInterface.*`, `Body/BodyLock*.h`, `Body/BodyFilter.h`, `Body/BodyActivationListener.h`, `PhysicsLock.h`,
`Constraints/Constraint.*`, `ConstraintManager.*`, `ContactConstraintManager.*`, `TwoBodyConstraint.*`, `MotorSettings.*`, `SpringSettings.*`, `CalculateSolverSteps.h`.
Зависит от: `MutexArray`, `SharedMutex`.

#### Этап F. Типы констрейнтов
`PointConstraint`, `FixedConstraint`, `DistanceConstraint`, `HingeConstraint`, `SliderConstraint`, `ConeConstraint`, `SwingTwistConstraint`, `SixDOFConstraint`,
`GearConstraint`, `RackAndPinionConstraint`, `PulleyConstraint`, `PathConstraint` (+ `PathConstraintPath*`).

#### Этап G. Основной цикл симуляции
Файлы: `PhysicsSystem.*`, `PhysicsUpdateContext.*`, `IslandBuilder.*`, `LargeIslandSplitter.*`, `PhysicsStepListener.h`, `PhysicsScene.*`, `StateRecorder.h`, `StateRecorderImpl.*`, `DeterminismLog.*`.
Зависит от: этапы A-F, **JobSystem** (уже есть), `TempAllocator`, FP-управление.
Тест: `UnitTests/Physics` (симуляция N шагов, `SaveState`/`RestoreState`, хэш состояния для детерминизма).

#### Этап H. Запросы и персонаж
`Query` (ray/shape/point casts через `NarrowPhaseQuery` и `BroadPhaseQuery`), `Character/CharacterBase`, `Character`, `CharacterVirtual`, `CharacterID`.

#### Этап I. Сериализация (если нужна)
`ObjectStream/*` (бинарная часть: `ObjectStreamBinaryIn/Out`, `ObjectStreamIn/Out`, `SerializableObject`, `SerializableAttribute*`, `TypeDeclarations`, `GetPrimitiveTypeOfType`).
Текстовые `ObjectStreamText*` можно не переносить. Зависит от RTTI/Factory/Streams.

#### Этап J. Debug-рендер
`Renderer/*`. Зависит от вашего графического слоя. Делать последним, прикрутить к `Physics.Debug`.

#### Этап K (позже, опционально)
`Vehicle/*`, `Ragdoll/*` + `Skeleton/*`, `SoftBody/*`. `Hair` и `Compute` не переносить.

### 4.4 Интеграция с остальным движком (не часть порта)

- Связь «тело Jolt <-> сущность ECS/узел `Engine.Node`» делает отдельный адаптерный слой. Код Jolt о ECS ничего знать не должен. Математику адаптер не конвертирует: физика и движок используют одни и те же `Vector3/Matrix4/Quaternion` (1.12).
- Ввод слоёв (`ObjectLayer`, `BroadPhaseLayer`) задаётся проектом (как `Layers.h` в Jolt Samples). Не хардкодить в портированном коде.
- `JobSystem` из Kernel передаётся в `PhysicsSystem::Update` как в оригинале.

### 4.5 Что решить заранее

1. Нужна ли сериализация шейпов/констрейнтов (разбирает RTTI/Factory/ObjectStream): если нет, этапы I и половина Streams откладываются.
2. 2D: отдельная реализация или `Plane2D` на 3D.
3. Двойная точность мира (`Real = double`): если нет, `DVec3`/`DMat44` не нужны.
4. Нужны ли Vehicle/Ragdoll/SoftBody в первой версии.
5. Расположение GJK/EPA (3.3).
6. Детерминизм между платформами (влияет на всё, см. начало раздела 2).
7. Память: заменить глобальные `operator new/delete` в `Kernel.Memory` (рекомендация: порт Jolt остаётся с обычным `new`) или перейти на явные `KernelMemory::Make/Destroy` (1.10, `JPH_OVERRIDE_NEW_DELETE`).
8. Где живут обёртки интринсиков: рекомендация `Kernel.Compiler` (3.1, п.4); если прототип не взлетит, бэкенды по архитектуре выбирает xmake.
9. Окончательные имена математических типов Jolt в Empire (таблица в 3.1, п.2) и имена динамических `Vector/Matrix/DynMatrix` (3.2).
10. Форма регистрации RTTI без макросов в `Kernel.Reflection` (расширить `TypeRegistry` или рядом, 2.1 `RTTI.h`).

------------------------------------------------------------------------------------------------------------------------

## 5. Сводная очередность (для планировщика)

| # | Что | Раздел | Блокирует |
|---|---|---|---|
| 1 | Решения (детерминизм, FMA, Real, ObjectLayer) как `constexpr` в `Kernel.Config`/`Kernel.Compiler`; убрать `*_API`/`_EXPORT` из заглушек и xmake | 1.3, 2 | всё |
| 2 | FP-управление | 2.1 | JobSystemThreadPool, физика |
| 3 | `Semaphore`, `SharedMutex`/`Locks`, `MutexArray` | 2.1 | BodyManager, BroadPhase, ThreadPool |
| 4 | Недостающие методы `Array`/`HashMap`, `Result`, `StridedPtr`, `ObjectToIDMap`, `Prefetch` | 2.1, 1.5 | почти всё |
| 5 | SIMD-обёртки в `Kernel.Compiler` (прототип) + `Math.h`, `Trigonometry`, `Real` | 3.1, 3.2 | Vec/Mat/Quat |
| 6 | `Vec4`, `UVec4`, `BVec16`, `Vec3`, `Quat`, `Mat44` (+ `DVec3`, `DMat44` при double): каждый заменяет движковый аналог, потребители переводятся сразу (3.5) | 3.2 | геометрия, физика |
| 7 | Остальная математика (`Matrix`, `DynMatrix`, `Gauss`, `Eigen`, `FindRoot`) | 3.2 | SixDOF, SoftBody |
| 8 | Геометрия (простые алгоритмы, `AABox4`, `ConvexHullBuilder`, GJK, EPA) | 3.3 | шейпы, коллизии |
| 9 | `TickCounter`, `StringTools`, Streams, RTTI/Factory | 2.1 | сохранение, ObjectStream |
| 10 | Physics этапы A-H | 4.3 | |
| 11 | Profiler, ObjectStream, Debug-рендер, Vehicle/Ragdoll/SoftBody | 2.1, 4.3 | |

------------------------------------------------------------------------------------------------------------------------

## 6. Что не проверялось

- Сборка и тесты проверены на Linux x86_64 для GCC/Clang и sanitizer-конфигураций (см. 2.6); другие платформы и компиляторы не проверялись.
- Полная построчная сверка всех исходников Jolt и Empire не выполнялась; текущая сверка покрывает состав файлов, документированный API и тестовые сценарии.
- `Cocoa`, `UIKit`, `WASM` бэкенды Platform (к переносу Jolt не относятся).
- `Empire.Math`: реализации (`.cpp`), только заголовки и состав файлов.
- Содержимое `Jolt/Physics/Collision/Shape` и `BroadPhase` по строкам (только список файлов), поэтому для них строк в таблице нет.
- Прототип SIMD-обёрток в `Kernel.Compiler`: компилируются ли ветки `if constexpr` с интринсиками отсутствующей ISA на gcc и clang (3.1, п.4).
- Правило экспорта на Windows (`utils.symbols.export_all`) на xmake 3.1.1 и сборка под Windows вообще.
- Потребители математики искались по подключениям заголовков (`Transform.3D`, `Geometry.3D`, `Math.Unit`); использование через промежуточные заголовки и в `.cpp` Graphics/Runtime глубоко не проверялось.


### 2.4 Дополнительная сверка API (2026-11-19)

- `JobSystemThreadPool`: исходный Jolt публично предоставляет `SetNumThreads`. В Empire ранее метод был переименован в `SetThreadCount`; добавлен `SetNumThreads`, делегирующий существующей реализации, чтобы сохранить совместимость публичной семантики без дублирования логики.
- `ObjectToIDMap.h`: исходное имя второго alias — `IDToObjectMap` (заглавные `ID`). В Empire уже существовал стильный `IdToObjectMap`; добавлен точный `IDToObjectMap` alias на тот же тип, старое имя сохранено.
- В `Unit.Kernel.cpp` добавлены compile-time проверки доступности обоих alias-имён и `JobSystemThreadPool::SetNumThreads`. Тесты/сборка не запускались по просьбе пользователя до окончания переноса.

- `StreamIn::Read(Array<T>&)`: после bulk `ReadBytes` проверка теперь учитывает как `IsEOF()`, так и `IsFailed()`, чтобы корректно очищать массив при реализации потока, которая сигнализирует о коротком чтении через EOF отдельно от IO failure. Это соответствует контракту `StreamIn::IsEOF` в Jolt и не полагается на поведение только `StreamInWrapper`.

- `Array`: `End/end` теперь возвращают исходный `Items`, когда массив пуст, вместо вычисления `nullptr + 0`. Это убирает неопределённую pointer arithmetic для default-constructed пустого массива; добавлена unit-проверка равенства `Begin/End`, `begin/end` и `cbegin/cend` на пустом массиве.
- `MutexArray`: профильные scopes намеренно не подключаются в `Kernel.Sync`, поскольку `Kernel.Profiler` зависит от `Kernel.Sync`; включение создаёт циклическую зависимость модулей. Функциональность LockAll/UnlockAll перенесена, профилирование для этих методов опущено.

- Сверка `JobSystem.h` и `JobSystemWithBarrier.h` с Empire: публичные возможности `JobHandle`, dependency batching, barrier callbacks, job state transitions и release/acquire handoff уже представлены; отличия имён (`sRemoveDependencies` -> `RemoveDependencies`) соответствуют правилу Empire об отсутствии `s`-префикса. В этом проходе дополнительных изменений в алгоритм очереди/барьера не вносилось.

- `TempAllocator`: добавлен публичный `NeedsAlignedAllocate` (constexpr) по смыслу Jolt `needs_aligned_allocate`, вычисляемый относительно `KernelMemory::DefaultAlignment`. В Empire аллокатор всегда передаёт требуемое выравнивание непосредственно в `KernelMemory::Allocate`, поэтому ветвление по этому флагу не требуется. В `Unit.TempAllocator.cpp` добавлена compile-time проверка согласованности признака с выравниванием.

- `LockFreeHashMap`: сверены имена публичных методов Jolt и Empire. Добавлены точные PascalCase-эквиваленты `GetNumBuckets`, `GetMaxBuckets`, `SetNumBuckets`, `GetNumKeyValues`, делегирующие уже существующим `GetBucketCount`, `GetMaxBucketCount`, `SetBucketCount`, `GetKeyValueCount`. В `Unit.Concurrent.cpp` добавлены compile-time проверки доступности. Поведение хранения/очередей не менялось.

- `JobSystemThreadPool` / Apple threading: Jolt на macOS/iOS явно переносит QoS-класс создателя на worker threads. В `Threading.MacOS` добавлено сохранение `qos_class_self()` в StartData и применение `pthread_set_qos_class_self_np(..., 0)` в trampoline до вызова пользовательской функции. Это переносит поведение на уровень выбранного macOS backend без платформенных `#if` в общем Kernel-коде. На других платформах поведение не меняется.

- `LockFreeHashMap::TraceStats`: диагностическая статистика Jolt адаптирована как `GetStatistics()`, возвращающий число корзин/элементов, максимум цепочки и 256-биновую гистограмму. Возвращаем данные вместо вызова глобального логгера, чтобы не создавать циклическую зависимость Kernel.Containers -> Kernel.Logging -> Kernel.Containers; добавлены compile-time и runtime проверки.

- `StridedPtr`: в исходнике Jolt публичный accessor называется `GetPtr()`, в Empire он уже назывался `GetPointer()`. Добавлен `GetPtr()` как делегирующий alias, оба имени сохранены; добавлена runtime unit-проверка адреса через оба метода.

- Исправлена обнаруженная ошибка в `StringTools::Split`: прежняя реализация не добавляла последний пустой элемент при завершающем разделителе, но оригинальный Jolt `StringToVector` всегда добавляет остаток строки после цикла, включая пустую строку. Теперь `"red::"` даёт `[`red`, ``]`, `"::"` даёт два пустых элемента; тесты обновлены и добавлен случай с несколькими разделителями в конце.

- `StreamInWrapper`: callback, возвращающий не ровно запрошенное число байт (как недочтение, так и некорректное превышение), теперь помечает поток как EOF/failed. Ранее проверялось только значение меньше запроса; добавлен unit-case на некорректное превышение счётчика чтения.

- `ByteBuffer`: закрыты дополнительные публичные возможности исходного `ByteBuffer` (наследуемые в Jolt от `Array<uint8>`): добавлены `AppendVector(Array<T>)`, индексирование байтов, `begin/end`, `Clear` и `IsEmpty`. `Get<T>` теперь проверяет смещение в Debug, как исходный `Array::at`; `Allocate<T>(0)` безопасно возвращает nullptr без арифметики над null. Добавлены unit-cases на добавление массива, итераторы, индексирование, очистку и нулевое выделение.

- `ByteBuffer` дополнен остальными используемыми операциями базового Array: публичный `Reserve`, `PushBack`, `PopBack`, `Front`, `Back`, `Size`, `Empty`, `Data`; в `PushBack` добавлена проверка переполнения размера. Тесты покрывают эти операции.

- Исправлен дефект времени жизни в `Array::Insert(index, value)`: при сдвиге элементов вправо исходный слот `Index` уже разрушался последней итерацией цикла, но затем повторно вызывался его деструктор перед конструированием вставляемого значения. Удалён лишний `DestroyAt` в обеих перегрузках (`const T&` и `T&&`); добавлен тест с отслеживанием числа живых объектов, чтобы ловить двойное разрушение.

- `FastArray`: исправлена неопределённая pointer arithmetic на пустом массиве (`end()` не вычисляет `nullptr + 0`). Обратные итераторы переведены на one-past base pointer, как `std::reverse_iterator`: разыменование берёт `Current - 1`, `rbegin` это `end`, `rend` это `begin`. Это убирает `Items - 1` для пустого массива и сохраняет ту же последовательность обхода; добавлены тесты пустых mutable/const iterators.

- Дополнительно проверены пустые пути `FastArray`: `Move(..., 0)` теперь сразу возвращается (вставка диапазона в конец не вычисляет `Destination - 1`), `Resize` не делает арифметику над null для пустого нетривиального массива, `GetHash` не строит `Items + 0`, а проверки алиасинга в `Resize/PushBack/Insert` выполняются только при выделенном буфере. `Grow` и `AllocateBlock` проверяют переполнение размера до умножения/сложения. Добавлены соответствующие unit-cases.

- Завершена обработка null-итераторов в пустых массивах: `Array::Insert(range)` и `FastArray::Insert(range)` корректно вставляют в пустой массив без вычитания двух null-указателей; пустые `Erase(range)` и `FastArray::Assign(empty range)` завершаются до вычисления расстояния. Добавлены unit-cases для вставки в пустые `Array/FastArray`, пустого Assign и Erase.

- `FastArray::Reallocate` теперь проверяет переполнение `NewCapacity * sizeof(T)` до обеих ветвей — как при обычном выделении, так и при `KernelMemory::Reallocate` уже существующего блока.

- Статус `Array.h`/`MutexArray.h` в таблице обновлён до `[x]`: публичная функциональность этих компонентов перенесена с адаптациями под Kernel API; оставшиеся проверки сборки/потребителей намеренно оставлены на пользовательский этап после завершения всего Core.

- Исправлена зависимость между модулями: первоначальный вариант `LockFreeHashMap::TraceStats()` включал `Kernel.Logging` из заголовка `Kernel.Containers`, что создавало бы цикл зависимостей, поскольку Kernel.Logging уже зависит от Kernel.Containers. Теперь диагностика возвращается структурой `LockFreeHashMapStatistics` через `GetStatistics()`; логирование остаётся на стороне вызывающего слоя. Проверка гистограммы добавлена в `Unit.Concurrent.cpp`.

- Удалены ранее добавленные `KernelProfiler::Scope` из `MutexArray`: проверка xmake-графа показала цикл `Kernel.Sync -> Kernel.Profiler -> Kernel.Sync`. Это не должно оставаться в проекте; профилирование `MutexArray` опущено, как опциональная макро-инструментация Jolt.

- Обновлён граф модулей: `Kernel.Containers` теперь явно зависит от header-only `Kernel.Compiler`, поскольку `Containers.FastHashTable.hpp` использует `EqualMask16`/`CountTrailingZeros` из Compiler. Цикла зависимостей нет (`Kernel.Compiler` не зависит от Containers).

- `MutexArray` больше не включает `Kernel.Containers/Containers.Hash.hpp`: FNV-1a по байтам `UInt32` реализован локально и проверяется тестом на равенство с `KernelContainers::Hash<UInt32>`. Это убирает лишнюю зависимость `Kernel.Sync -> Kernel.Containers` (которая была бы циклической через `Kernel.Containers -> Kernel.Sync`). Зависимость от `Kernel.Memory` остаётся необходимой для выделения массива; потребителям `MutexArray` следует линковать `Kernel.Memory`.

- Уточнены xmake-зависимости для публичных заголовков: `Kernel.Memory` теперь зависит от header-only `Kernel.Compiler` (его используют `Memory.TempAllocator.hpp` и `Memory.Reference.hpp`), `Kernel.System` — от `Kernel.Compiler` (Windows/UWP backend). Циклов не возникает.

- `Profiler`: добавлено наследование цвета вложенных scope по правилу Jolt (`Color == 0` наследует цвет родителя). Предыдущий цвет сохраняется в `Scope` и восстанавливается при выходе, включая переполнение sample buffer; это работает для произвольной глубины без стека фиксированного размера. Unit-сценарий проверяет наследование, переопределение цвета вложенного scope и восстановление цвета для следующего sibling.

- Цвет по умолчанию в `Profiler` установлен в Jolt `sGetDistinctColor(0)` (красный, packed `0xff0000ff`); теперь scope с `Color == 0` на верхнем уровне получает этот цвет, а вложенные scope наследуют цвет родителя. В тесте добавлена отдельная проверка цвета по умолчанию.

- Обратные итераторы `Array` и `FastArray` дополнены random-access операциями, которые предоставляет `std::reverse_iterator` в исходном Jolt: signed offset, `operator[]`, разность итераторов и сравнения порядка. Разность двух одинаковых пустых итераторов возвращает 0 без вычитания null-указателей. Добавлены проверки индексирования и расстояния.

- Выполнена статическая проверка графа xmake для 24 targets (`Engine.Kernel` + `Kernel.Unit`): циклических `add_deps` не обнаружено. По include-scan единственный необъявленный модульный include остаётся у optional `Kernel.Sync/Sync.MutexArray.hpp -> Kernel.Memory`; это намеренно не превращено в зависимость target, поскольку `Kernel.Memory` сам зависит от `Kernel.Sync`. Потребитель `MutexArray` должен явно подключить `Kernel.Memory`. Это не была сборка и не запуск тестов.

- `FastArray::Move` также завершает работу при `Destination == Source`, не выполняя self move-construction поверх живых объектов; текущие внутренние вызовы не требуют такого перемещения, но guard делает helper безопаснее.

- Начат отложенный SIMD/NEON этап внутри `Kernel.Compiler` (без перехода к Math): добавлен `Compiler.Simd.Neon.hpp` с ARM64 vector constructors (`MakeInt32x4/MakeUInt32x4/MakeInt8x16/MakeUInt8x16`), Jolt shuffle specializations для MSVC/старого GCC и `__builtin_shufflevector` для новых GCC/Clang. Все JPH macros заменены функциями/templates; `<arm64_neon.h>`/`<arm_neon.h>` и архитектурные guards находятся только в `Kernel.Compiler`. На x86 header inert. ARM64 compile/runtime verification не запускалась и не требуется для завершения Kernel по решению пользователя.

- Добавлен ARM64-only compile-check source `Unit.Compiler.Neon.cpp` (не запускался): он инстанцирует vector constructors, builtin shuffle и generic/specialized fallback templates. На не-ARM64 функция пуста, поэтому обычная сборка не требует NEON headers.

- `RISCVVector.h` также перенесён в `Kernel.Compiler/Compiler.Simd.RISCV.hpp` под `__riscv_vector`: добавлены generic/specialized shuffle и `RVVSumElementsFloat32x4<Deterministic>` вместо `JPH_CROSS_PLATFORM_DETERMINISTIC` macro. В `Compiler.hpp` добавлен `SupportsRISCVVector`; ARM/RISC-V headers и guards остаются только в Compiler. Добавлен RISC-V-only compile-check source `Unit.Compiler.RISCV.cpp`; RISC-V toolchain/runtime verification не запускается и не требуется для завершения Kernel по решению пользователя.

- В `Kernel.Compiler` добавлен отдельный `SupportsARM64NEON`: общий `SupportsNEON` остаётся признаком NEON ISA (включая ARM32), а конкретные ARM64 shuffle helpers доступны только на ARM64. Это синхронизирует feature flags с guards `Compiler.Simd.Neon.hpp`.

- `Kernel.Unit` получил прямую xmake-зависимость `Kernel.Compiler`: новые ARM64/RVV compile-check translation units включают `Compiler.hpp` напрямую, и это не должно зависеть от транзитивного включения через Containers/Memory.

- `Kernel.Compiler::CpuAddressBits` теперь явно распознаёт RV64 через `__riscv_xlen == 64` (даже если toolchain не задаёт `__LP64__`); RV32 остаётся 32-битным.

- В `Kernel.Compiler` добавлен `IsRISCVArchitecture` по `__riscv`, чтобы архитектурная идентификация RV32/RV64 была симметрична x86/ARM; `SupportsRISCVVector` отдельно показывает наличие V extension.

- Архитектурные флаги `IsArmArchitecture`, `CpuAddressBits` и `VectorAlignment` также учитывают `_M_ARM64` (MSVC/clang-cl ARM64), чтобы не классифицировать эту платформу как 32-bit/не-ARM.

- Для RV64 выравнивание `DoubleVectorAlignment` приведено к Jolt (`32` байта); RV32 сохраняет `8` байт. `VectorAlignment` остаётся `16` байт на обоих RV-вариантах.

- Архитектурные guards ARM64 также учитывают `_M_ARM64EC`, как Jolt Core; ARM64EC использует те же NEON helpers и 64-bit/vector alignment properties.

- ARM64/RISC-V compile-checks расширены до runtime lane-order checks: constructors, generic/specialized shuffles и deterministic/fast RVV reductions проверяют фактические значения; на других архитектурах проверки возвращают `true` без обращения к ISA. Эти проверки добавлены, но не запускались.

- После добавления `Kernel.Compiler` в прямые зависимости `Kernel.Unit` повторная статическая проверка графа всех 24 targets (`Engine.Kernel` и `Kernel.Unit`) подтвердила отсутствие циклов. Сборка и тесты не запускались.


### 2.6 Контрольная точка: функциональный перенос Kernel

- Весь применимый non-Math объём `Jolt/Core` распределён по `Empire.Kernel`; намеренно исключены только STL allocator wrappers, `IncludeWindows.h` и sanitizer suppression headers (см. статусы `[-]` в таблице 2.1).
- Функциональные адаптации, которые ранее оставались `[~]`, закрыты: IssueReporting callbacks; per-thread profiler buffers, thread names and HTML report; abstract RTTI metadata; stream object serialization callbacks; StringTools formatting/conversion and exact Split/Join edge-case semantics.
- FMA отключён (`Kernel.Config::UseFMA == false`); `make.lua` запрещает неявное FMA-свёртывание через `-ffp-contract=off` на GCC/Clang и `/fp:strict` на MSVC. `CrossPlatformDeterministic` остаётся `false`; `-ffast-math` и `-march` не включаются.
- Добавлены тесты RAII-регистра FP и processor tick counter; проверки `StreamWrapper` покрывают EOF, короткое чтение, отсутствующий callback и нулевой размер. `JobSystemWithBarrier` покрыт `Kernel.Unit` сценариями барьера, включая добавление дочерних jobs во время ожидания.
- ARM64 NEON и RISC-V Vector находятся в `Kernel.Compiler`. Их специализированная компиляция на других архитектурах/системах не входит в критерий завершения и не будет блокировать порт.
- Math-only overloads `Vec3`, `DVec3`, `DMat44` для StreamIn/StreamOut и SSE/AVX/FMA vector math остаются в последующем Math этапе; это не Kernel/Core пропуски.
- `ObjectStream` и сериализуемые RTTI attributes являются отдельным optional subsystem вне текущего Kernel scope; базовые Core stream utilities и Factory type-hash callbacks перенесены.
- Linux x86_64 verification: полный набор targets `Kernel.*` (включая Environment, FileSystem, Library, Network и Process) собран; `Kernel.Unit` прошёл на GCC 16.2.1 Debug и Shipping, Clang 23.1.4 Debug, GCC ASan+UBSan и GCC TSan. Все прогоны завершились с 0 failures.
- Physics golden-hash determinism test остаётся для Physics этапа: симуляция и Physics API ещё не перенесены. ARM64/RISC-V и Windows/macOS не собирались.
- После Linux-проверки Kernel пользователь разрешил перейти к Math; проверка этого Kernel архива на основной платформе остаётся за пользователем.
- После разрешения пользователя начат этап Math. В `Math.Scalar` добавлена часть Jolt scalar utilities, а `Real` связан с `Kernel.Config`; введены storage-типы `Float2/3/4` и `Double3` с проверками layout. `Vector3/4` получили 16-byte alignment, accessors и scalar API. Это переходный этап, не завершённый SIMD-порт: API типов ещё не сверен построчно, потребители не мигрированы.
- `BVector16` перенесён в `Math.Vector` как 16-байтовый выровненный тип: конструкторы, загрузка, equality/logical masks, индексирование, `GetTrues` и проверки any/all truth. Текущая реализация portable/scalar; SIMD-ускорение будет добавлено через `Kernel.Compiler`, поэтому этот этап не считается завершённым Jolt SIMD-портом.
- `UVector4` получил порт scalar API Jolt 5.7.0: операции над компонентами и масками, load/store/aligned paths, gather с byte-scale и безопасной невыравненной загрузкой через `Kernel.Compiler`, свёртки, shifts, swizzle/splats и unpack. Проверки layout и публичного поведения добавлены в `Math.Unit`. SIMD backend и финальная построчная сверка ещё впереди.
- Добавлены `HalfFloat`, константы и scalar-конвертации binary16 <-> float с округлением к ближайшему-even и направленным округлением. Тестируется полный round-trip набора из 65 536 half-patterns (NaN приводится к Jolt canonical quiet NaN), а также границы subnormal, overflow, signed zero, infinity и NaN. SIMD/пакетная конвертация ещё не портирована.
- Добавлен тип `SwizzleComponent` с X/Y/Z/W и `Unused = Z`; `UVector4::Swizzle` покрыт тестом на перестановку lane.
- Для `Math.Scalar` и `Math.Vector` объявлены непосредственные xmake-зависимости по используемым модулям. `./Empire.Scripts/Make.sh Build Engine Test Math.Unit -j8` и `./Empire.Scripts/Make.sh RunTests -j8` прошли; после проверки инварианта `Vector3` повторный полный `RunTests` также прошёл (19/19 наборов). Сборки выполнялись через MakeTool, не xmake CLI.
- Расширен scalar API `Vector4`: загрузка/store/gather, операции и маски, sorting, конверсии, reductions, bit operations и compression/decompression unit-вектора по Jolt 5.7.0. Перенесены Vec4 SinCos/Tan/ASin/ACos/ATan/ATan2 и scalar wrappers; тесты проверяют векторные lane, квадранты и численные тождества. SIMD-путь и построчная сверка ещё впереди.
- `Vector3` заменён на Jolt-подобный scalar API с приватными компонентами и accessors, storage/Vector4 constructors, component masks/operations, predicates, swizzle/splats, reductions, normalized-perpendicular, `sRandom`, 1026-point unit sphere, precise cross product и 14-bit unit-vector compression. Добавлены проверки random vectors, всех точек сферы и 512 round-trip компрессий; `./Empire.Scripts/Make.sh RunTests Math.Unit -j8` прошёл: 19/19 targets. SIMD backend и полная сверка Jolt API ещё не сделаны.
- По просьбе пользователя удалены прежние реализации неперенесённых модулей Math, оставлена структура директорий, а Jolt-перенесённые файлы переименованы без двойного префикса (`Vector4.hpp`, `UVector4.hpp`, `Float3.hpp`, `HalfFloat.hpp` и т.д.). Legacy-тесты Matrix/Quaternion/Transform/Geometry и Vector2 удалены; Math.Unit оставлен для текущего набора портов. MakeTool `RunTests Math.Unit` после очистки прошёл: 19/19 targets.
- Начаты scalar-порты `Matrix4` и `Quaternion`; добавлены quaternion tests и расширены matrix regression checks. По результатам сверки исправлены `Mat44::GetRotation`/`GetRotationSafe`, `PostScaled`, signed modified Gram-Schmidt decomposition, 3x3 inverse bottom-row behavior, scalar 4x4 inverse и quaternion left/right multiplication matrix. MakeTool `RunTests Math.Unit -j8` прошёл: 19/19 targets. Это промежуточный статус, не завершение переноса Math.
- `Math.Scalar/FindRoot.hpp` портирует Jolt `FindRoot.h` (квадратные, линейные и константные уравнения) и включён в Math.Unit. MakeTool `RunTests Math.Unit -j8` после переноса прошёл: 19/19 targets.
- `Math.Scalar/MathTypes.hpp` добавил `Real3`, `RVector3/RMatrix4` и аргументные aliases, выбираемые через `KernelConfig::WorldUsesDoublePrecision`; double-precision типы объявлены вперёд и будут реализованы в отдельном этапе.
- В `Math.Vector/HalfFloatConversion.hpp` добавлен скалярный packed-4 half-to-float fallback поверх `UVector4`; Math.Unit проверяет все 65536 битовых паттернов, распределённые по четырём lanes. MakeTool `RunTests Math.Unit -j8` прошёл: 19/19 targets.
- `Math.Matrix/{Vector,Matrix,DynMatrix}.hpp` и `GaussianElimination.hpp` портируют generic fixed-size/dynamic storage и Gauss-Jordan inverse. `EigenValueSymmetric.hpp` портирует Jacobi eigensolver; добавлены проверки non-square multiply, 2x2/4x4 inverses и symmetric eigenpairs. MakeTool suite прошёл: 19/19 targets, включая Math.Unit.
- `Geometry.3D/AABB.hpp` расширен основными float-операциями, plane overlap и triangle/indexed-triangle construction/encapsulation; добавлены `Plane.hpp` (float plane operations and three-plane intersection), `Sphere.hpp` (support, overlap, encapsulation), `OBB.hpp` (SAT против AABB/OBB), `Ellipse.hpp`, `MortonCode.hpp`, `Triangle.hpp` и `IndexedTriangle.hpp`. Тесты покрывают geometry cases, включая rotated OBB, indexed triangle and 10-bit Morton endpoints. MakeTool `RunTests Math.Unit -j8` прошёл 19/19; в отдельных запусках ранее наблюдался нестабильный `Kernel.Unit` результат запускающего runner, хотя самостоятельный `Kernel.Unit` проходит.
- В `Geometry.3D/RayAABox.hpp`, `RaySphere.hpp`, `RayCylinder.hpp`, `RayCapsule.hpp` и `RayTriangle.hpp` реализованы scalar пересечения лучей с соответствующими примитивами (включая ray-box интервалы и цилиндр с плоскими крышками); для AABB и triangle добавлены 4-lane API fallback implementations. Проверяются пересечения, промахи, параллельность и случаи старта внутри. MakeTool `RunTests Math.Unit -j8` прошёл 19/19.
- В `Geometry.3D/ClosestPoint.hpp` портированы barycentric coordinates, closest-point на line/triangle/tetrahedron, plane-side checks и feature masks, включая fallback для вырожденных треугольников. Тесты line, triangle face/vertex/degenerate edge и tetrahedron inside/outside прошли в MakeTool suite (19/19).
- `Geometry.3D/ClipPoly.hpp` добавил clipping полигона плоскостью, другим полигоном, ребром и AABB; проверены plane/polygon/AABB cases. `Geometry.3D/AABB4.hpp` содержит пакетные four-lane пересечения с AABB/point/sphere/OBB, distance, scaling и расширение границ; OBB lanes пока считаются скалярно. `Geometry.3D/Indexify.hpp` выполняет weld вершин и обратное разворачивание triangle list, сохраняя материал/user data и порядок представителей. MakeTool `RunTests Math.Unit -j8` подтверждает suite 19/19.
- `Geometry.3D/ConvexHullBuilder.cpp` now builds 3D hulls and reconstructs polygon faces from coplanar triangles. `Math.Unit` now includes the Jolt dense-cube, 2D hull, sphere, degeneracy, vertex-limit, and near-coplanar regression cases. The full MakeTool suite passes 19/19; the implementation still needs full source/API/numerical parity review before marking the Jolt row `[x]`.
- Runtime test binaries now add the sibling `Build.Engine` directory to their runtime search path in `make.lua`; this lets MakeTool launch tests that depend on Engine shared libraries without relying on externally supplied `LD_LIBRARY_PATH`.

### Дополнение 2026-11-20: закрытые пробелы ядра
- `Kernel.Sync`: добавлены `AtomicMin`/`AtomicMax` (нужны `QuadTree` и `IslandBuilder`), `Mutex::IsLocked` и `SharedMutex::IsLocked` (аналог `is_locked` Jolt).
- `Kernel.Reflection`: у `TypeInfo` появились атрибуты членов (`AddAttribute<&Класс::Член>(Имя)`, `GetAttributeCount`, `GetAttribute`, `FindAttribute`, `TypeAttribute::GetValue`), без макросов. `SetName` не переносился: имя задаётся при регистрации в `TypeRegistry`, переименование ломало бы индексы по имени и хэшу.
- Не входит в Core и не переносилось: `SerializableObject`, `RestoreFromBinaryState`, ObjectStream.
- Удалён пустой `Kernel.Config/Config.cpp` (таргет `headeronly`).
- `Unit.Jobs.cpp`: `TestStress` использует пул на 8192 задач, чтобы не зависеть от скорости воркеров на машинах с одним ядром.
- Linux x86_64, g++ 13, `-std=c++23 -Wall -Wextra`: Math 93967 проверок, Kernel 100235 проверок, 0 провалов, предупреждений нет.
