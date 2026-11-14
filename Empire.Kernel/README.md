# Empire.Kernel: полный анализ

Состояние на 2026-11-14. Это рабочий список, а не описание готовой архитектуры.
Namespace и имена файлов повторяют пути папок (см. `../ARCHITECTURE.md`).

Модули: `Compiler`, `Types`, `System`, `Sync`, `Threading`, `Time`, `Process`, `Environment`, `FileSystem`,
`Library`, `Network`, `Assert`, `Memory`, `Containers`, `Logging`, `Events`, `Services`, `Reflection`, `Config`.

## Итог

- **std из Kernel убран полностью.** Он был (`std::cin/cout/cerr`, `std::string`, `std::allocator` и др.), последние следы
  исчезли в `v1.1.3`. Сейчас ни один файл Kernel (кроме `System.Linux.cpp`) не подключает ни одного системного заголовка.
- **Поведение ядра исправно:** поведенческий тест (100 120 проверок) проходит под ASan, UBSan, TSan, gcc -O3 и clang -O2,
  утечек нет.
- **Найденные проблемы (раздел 3) исправлены.** Главная была в аллокаторе (не масштабировался по потокам): теперь у каждого потока свой кэш блоков.
- **Много недописанного:** 8 пустых модулей и ряд базовых вещей, которых нет по сравнению со std (разделы 4 и 5).

## 1. История std в Kernel (по git)

| Коммит | Дата | Что было |
|---|---|---|
| `v1.0.2` | 2026-10-17 | `Types.hpp`: `<iostream>`, `std::cin/cout/cerr` (как `Cin`, `Cout`, `Cerr`), `std::string`, `std::string_view`, `std::int8_t`..`std::uint64_t`, `std::size_t`, `std::byte` |
| `v1.0.4` | 2026-11-09 | + `std::runtime_error` как `Error`, `std::to_string` в `ToString` |
| `v1.1.0` / `v1.1.1` | 2026-11-12 | Kernel переписан: 17 файлов и ~211 строк с `std::` (`std::allocator`, `std::construct_at`, `std::move`, `std::initializer_list`, `std::source_location`, `std::fprintf`, `std::abort`, `std::length_error`, `std::out_of_range` и т.д.) |
| `v1.1.2` | 2026-11-13 | осталось 8 строк: `std::abort` (Assert), `std::malloc/free` (Memory), `std::initializer_list` (Array), `<new>` (Types.Utility) |
| `v1.1.3` | 2026-11-13 | осталась 1 строка (комментарий про `std::initializer_list` в `Array.hpp`): аллокатор переписан поверх страниц ОС, `abort` ушёл в `System::Terminate`, `<new>` заменён собственным placement-new |

## 2. Как проверено, что std сейчас нет

1. **Поиск по коду:** нет ни одного `std::`, `cout`, `cerr`, `cin`, `printf`; единственное упоминание это комментарий в `Containers.Array.hpp`.
2. **Список заголовков от компилятора** (`g++ -H`): все `.cpp` и все `.hpp` Kernel (кроме `System.Linux.cpp`)
   подключают **ноль** системных заголовков. `System.Linux.cpp` подключает только libc и POSIX; оговорка: g++ подменяет
   `<stdlib.h>` на свою обёртку, которая тянет `<cstdlib>`, но кода на `std::` там нет.
3. **Сборка с `-nostdinc++`:** все `.cpp` Kernel компилируются без заголовков стандартной библиотеки C++.

**Но рантайм C++ нужен.** `throw` это возможность языка, а не аналог std, и ему нужны библиотеки ABI
(libstdc++ / libsupc++). В объектных файлах остаются неопределённые символы:

| Откуда | Символы | Где в коде |
|---|---|---|
| исключения | `__cxa_allocate_exception`, `__cxa_throw`, `__gxx_personality_v0`, `_Unwind_Resume` | 33 `throw` в Reflection (8), Array (7), Memory (4), String (4), HashMap (4), ServiceRegistry (2), SharedPtr, EventBus, StringView, Function (по 1) |
| `try`/`catch` | `__cxa_begin_catch`, `__cxa_end_catch`, `__cxa_rethrow` | `Reflection.TypeRegistry.cpp` |
| статические локальные переменные | `__cxa_guard_acquire`, `__cxa_guard_release`, `__cxa_atexit` | `Logging.cpp` (`static ConsoleSink DefaultSink`) |
| наследование и `virtual` | `typeinfo`, `vtable for __cxxabiv1::*`, `__cxa_pure_virtual` | `Assert` (иерархия ошибок), `Logging::Sink`, `Function`, `SharedPtr::ControlBlock` |

Полностью отказаться от рантайма можно только через `-fno-exceptions -fno-rtti -nostdlib++` и замену `throw` на
`Result`/`Optional`. Это решение надо принять заранее (раздел 6).

**Компиляторные конструкции вне `Kernel.Compiler`:** осталась одна, `__PRETTY_FUNCTION__` в `GetTypeKey()`
(`Kernel.Types/Types.Utility.hpp:202`). Её нужно вынести в `Kernel.Compiler`. Там же `new T(Value)` внутри `requires`
в `IsCopyConstructible`: допустимо, но лучше заменить на `IsConstructible`.

**`std` в документации:** `Empire.Platform/README.md` (наброски XCB, часть II) содержит `std::cerr`, `std::unordered_map`,
`std::runtime_error`, `std::string`, `std::queue`, `std::strlen`, `std::free`. При переносе заменять на:

| В наброске | Аналог в Kernel |
|---|---|
| `std::cerr` | `KernelLogging::Error(...)` |
| `std::unordered_map` | `KernelContainers::HashMap` |
| `std::runtime_error` | `KernelAssert::Error` и наследники |
| `std::string` | `KernelContainers::String` / `StringView` |
| `std::queue` | нет аналога (нужна `Queue`, раздел 5) |
| `std::strlen`, `std::free` | `StringView(const Char*)`, `KernelMemory::Deallocate` |

## 3. Поведение и найденные проблемы

Тест лежит в `Empire.Tests` (использует std только для потоков и вывода). Проверено: `Types`, `Memory`, `Array`,
`HashMap`, `String`/`StringView`, `SharedPtr`/`Function`, `EventBus`, `ServiceRegistry`, `TypeRegistry`, `Logging`,
`Assert`, 8 потоков на `Mutex`/`Atomic`/`EventBus`/`ServiceRegistry`/аллокаторе. Включены границы (переполнения размеров,
нулевые указатели, самоприсваивание, добавление элемента самого массива при реаллокации, исключение из копирования
элемента: счётчик живых объектов и блоков памяти возвращается к нулю).
Результат: 0 провалов, 0 замечаний ASan/UBSan/TSan, утечек нет (тест лежит в `Empire.Tests/Tests.Unit`).

Найденные проблемы (все исправлены):

1. **Аллокатор не масштабировался по потокам и был медленнее `malloc`.** Был один глобальный `PoolMutex` на все классы размеров,
   три общих атомарных счётчика на каждый вызов, линейный поиск класса и заголовок 48 байт. Исправлено в два шага.
   Шаг 1: у каждого потока свой кэш блоков (пачки по 32 из общего пула, мьютекс только при пополнении и сбросе), при
   завершении потока кэш возвращается в пул. Шаг 2 (ускорение до уровня `malloc`):
   - заголовок блока 16 байт вместо 48 (размер и класс; данные больших блоков вынесены в `LargeInfo` перед заголовком);
     указатель на следующий свободный блок лежит в полезной части, поэтому класс пишется в заголовок один раз при нарезке;
   - класс размера ищется по таблице, а не циклом;
   - счётчики живых блоков/байт у каждого потока свои, пишет только владелец (обычные load/store без lock-префикса),
     сумма по всем потокам остаётся точной (освобождение в другом потоке допустимо, поток-владелец может «уйти в минус»,
     сумма не страдает); блок счётчиков завершившегося потока остаётся в сумме и переиспользуется;
   - состояние потока `constinit thread_local` без конструктора, поэтому быстрый путь не проверяет инициализацию;
     деструктор потока вынесен в отдельный объект, который создаётся один раз при первом вызове;
   - для `Kernel.Memory` в `xmake.lua` включено `-ftls-model=initial-exec` (в `.so` доступ к TLS быстрее примерно в 2 раза).
   Публичный API не менялся.

   Шаг 3 (задержка хвоста): размер пачки зависит от класса (около 8 КиБ на пачку, от 2 до 32 блоков), в общем пуле пачки
   лежат целиком и берутся/отдаются за O(1) без обхода «холодных» блоков; при сбросе уходят самые свежие блоки.

   **Результаты** (бенчмарк `Empire.Tests/Tests.Benchmark`, `xmake run Empire.Bench`; 2 vCPU, лучшее из 5 запусков,
   время самого медленного потока, меньше = лучше). Колонки: «было» = аллокатор до моих правок (один общий лок),
   «вчера» = первая версия потоковых кэшей, «сейчас» = текущий, «malloc» = glibc.

   Тест 1, alloc+free по кругу (400k операций на поток), мс:

   | Потоков | было | вчера | **сейчас** | malloc |
   |---|---|---|---|---|
   | 1 | 19,7 | 14,2 | **2,9** | 3,9 |
   | 2 | 159,1 | 14,6 | **5,1** | 6,6 |
   | 4 | 185,0 | 32,3 | **5,1** | 7,9 |
   | 8 | 295,2 | 57,3 | **10,5** | 13,5 |

   Тест 2, смешанные размеры 16..2000 Б, 4096 живых блоков, 1M операций на поток, мс:

   | Потоков | было | вчера | **сейчас** | malloc |
   |---|---|---|---|---|
   | 1 | 51,2 | 44,1 | **9,7** | 34,2 |
   | 2 | 325,0 | 50,3 | **14,0** | 35,8 |
   | 4 | 493,4 | 93,7 | **28,8** | 78,6 |
   | 8 | 842,9 | 189,3 | **57,1** | 148,6 |

   Тест 3, задержка одного вызова, такты rdtsc (медиана по 5 запускам, 1 поток):

   | Перцентиль | было | вчера | **сейчас** | malloc |
   |---|---|---|---|---|
   | p50 | 120 | 100 | **46** | 44 |
   | p99 | 316 | 164 | **124** | 308 |
   | p99.9 | 744 | 738 | **278** | 592 |
   | p99.99 | 9178 | 9342 | **788** | 6024 |

   p50 равен malloc в пределах шума (в разных запусках 40–48 против 44–46). По скорости и по хвосту задержки Kernel
   лучше malloc во всех тестах. Дополнительно проверено: ядро как `.so` с `initial-exec` (тест 2: ×0,39–0,49 от malloc)
   и освобождение в другом потоке (производитель выделяет, потребитель освобождает, 1M блоков): Kernel 70–90 мс, malloc 450–600 мс.
   Оговорки: `malloc` это glibc с tcache (у него есть защиты от двойного освобождения и порчи, у Kernel их нет), на Windows
   и macOS цифры будут другими; замеры на 2 vCPU шумные.

   **Шаг 4, память.** Раньше малые блоки (до 2 КиБ) никогда не возвращались в ОС. Теперь есть `KernelMemory::Trim()`: возвращает ОС
   куски по 128 КиБ, все блоки которых свободны (учитывает свободные блоки в общем пуле и кэш вызывающего потока; кэши других
   потоков и живые блоки не трогает), и `GetReservedByteCount()` (сколько удерживается у ОС). `Trim` обходит весь пул свободных
   блоков, поэтому вызывать его стоит в удобный момент (смена уровня, пауза), а не в горячем цикле. Как страховка Trim срабатывает
   сам, если в пуле простаивает больше 256 МиБ (дальше порог удваивается, чтобы не обходить пул снова и снова).
   Быстрый путь Trim не затрагивает, скорость не изменилась.

   Память (200k блоков 16..2000 Б, запрошено 193 МБ; `Trim` вызывается после `free`):

   | | Kernel | malloc |
   |---|---|---|
   | RSS при живых блоках | +230 МБ (×1,19 от запрошенного) | +190 МБ (×0,98) |
   | RSS после free | +230 МБ | +190 МБ |
   | RSS после free и Trim | **+0 МБ** | +190 МБ (malloc сам память не возвращает) |

   Что осталось: накладные расходы Kernel на живых блоках на 20% больше (заголовок 16 байт плюс округление до класса);
   кусок, в котором остался хоть один живой блок, вернуть нельзя (например, 300k блоков по 64 Б, половину освободили:
   память не вернётся, у malloc так же: у обоих RSS около +175 МБ при следующих 150k блоках по 1 КиБ); без вызова `Trim`
   (или порога 256 МиБ) освобождённая память остаётся у аллокатора для повторного использования теми же классами размеров.
2. **`Mutex`:** добавлен `KernelCompiler::Pause()`; ожидание: 8 попыток с `Pause`, затем `YieldThread` до 2048 попыток, затем
   сон по 50 мкс. Сам `Mutex` на тесте с пустой секцией держится на уровне прежнего (шум замеров в песочнице велик),
   а «схлопывание» аллокатора ушло вместе с общим локом. `ConditionVariable` по-прежнему нет (новая функциональность, раздел 5).
3. **`SleepThread` в `System.Windows.cpp` и `System.UWP.cpp`:** деление до сложения, переполнения `UInt32` больше нет.
4. **`TypeRegistry`:** идемпотентная повторная регистрация того же типа с тем же именем описана в комментарии у `Register`.
5. **`Logging`:** путь в строке укорачивается (часть после последнего `Empire.Game.Engine/`, иначе имя файла); контракт
   «приёмник получает готовую строку с завершающим `\n`» записан в комментарии у `Sink`.
6. **`Terminate` в `System.Windows.cpp`:** вместо пустого цикла `KernelCompiler::Trap()`.
7. Заодно: `__PRETTY_FUNCTION__` перенесён в `KernelCompiler::TypeSignature<T>()`; `IsCopyConstructible` больше не использует
   `new` (`requires(const T& Value) { T(Value); }`).

Тесты (в том числе `Trim`, в том числе при параллельных выделениях) лежат в проекте: `Empire.Tests/Tests.Unit/Unit.Kernel.cpp` (поведение, `xmake run Empire.Tests.Unit`),
`Tests.Integration/Integration.Kernel.cpp` (потоки, `xmake run Empire.Test` гоняет оба), `Tests.Benchmark/Benchmark.Kernel.cpp`
(`xmake run Empire.Bench`).

Проверка после правок: поведенческий тест (100 120 проверок) под ASan+UBSan, TSan (0 предупреждений), gcc -O3, clang -O2;
интеграционный тест освобождения блоков в других потоках, коротких потоков и выделения в `thread_local`-деструкторе (ASan и TSan
чисто); синтаксис всех `.cpp` Kernel с `-nostdinc++`.

8. Хорошее: `Error` хранит сообщение в буфере 255 символов (обрезает, но не выделяет память, поэтому можно бросать
   даже при нехватке памяти); `GetTypeKey` даёт разные ключи для разных типов и стабильный для одного; `HashMap` выдерживает
   миллион циклов вставки/удаления одного и того же ключа без роста; исключения безопасны.

Что не проверено: правки Windows/UWP (`Sleep`, `Terminate`) не компилировались (нет окружения); бэкенды Windows, UWP, macOS, iOS, Android, WASM (нет окружения; macOS/iOS/WASM почти копии Linux,
Android отличается 24 строками); clang с санитайзерами (нет их рантайма в песочнице, clang -O2 без санитайзеров проходит).

## 4. Заглушки и недописанное

| Что | Состояние |
|---|---|
| `Kernel.Config` | пустой namespace, в `xmake.lua` модуля закомментирован |
| `Kernel.Time`, `Kernel.Threading`, `Kernel.FileSystem`, `Kernel.Process`, `Kernel.Library`, `Kernel.Environment`, `Kernel.Network` | пустые (только namespace), бэкенды Linux, Windows, MacOS тоже пустые |
| `Kernel.Reflection` | только имя, размер, выравнивание и ключ типа; полей и методов нет |
| `Kernel.Logging` | только `const char*` и `StringView`; форматирования нет (`ToString` есть, в лог не подключён) |
| `Kernel.Assert` | `Check` и `Fail` и иерархия исключений; режима «только в Debug» нет |
| `Kernel.System` | 7 бэкендов; Linux, macOS, iOS, WASM, Android почти копии друг друга |

## 5. Чего не хватает по сравнению со std

| Область | Есть | Нет (аналог std) | Важность для движка |
|---|---|---|---|
| Контейнеры | `Array` (≈`vector`), `HashMap` (≈`unordered_map`), `String`, `StringView`, `Function` | `Span`, `Pair`/`Tuple`, `Optional`, `Variant`, `Result`/`Expected`, `Deque`, `Queue`, `Stack`, `Set`, `Bitset`, `PriorityQueue`, `RingBuffer`, `Array<T, N>`, `List` | высокая: ECS, Jobs, Assets, Networking |
| Алгоритмы | `Min`, `Max`, `Swap`, `Move`, `Forward` | `Sort`, `BinarySearch`, `Find`, `Copy`, `Fill`, `Equal`, `Reverse`, `Clamp`, `Abs`, `Lerp` | высокая (`Abs`/`Clamp`/`Lerp`, возможно, в Math) |
| Типы и трейты | `RemoveReference`, `RemoveCV`, `IsSame`, `IsIntegral`, `IsFloatingPoint`, концепты `Integral`/`FloatingPoint`/`Arithmetic`, `IsSigned`, `IsCopyConstructible`, `IsNothrowMoveConstructible`, `MaxValue` | `IsPointer`, `IsEnum`, `IsClass`, `IsConvertible`, `IsBaseOf`, `IsConstructible`, `IsTriviallyCopyable`, `Decay`, `Conditional`, `EnableIf`, `UnderlyingType`, `InvokeResult`, `BitCast`, пределы для float (Epsilon, Infinity), битовые операции (`Popcount`, `CountLeadingZeros`, `Rotate`) | средняя |
| Память | `Allocate`, `SharedPtr`, `MakeShared`, `StaticPointerCast` | `UniquePtr`, `WeakPtr`, интерфейс `Allocator`, арены и пулы | высокая для `UniquePtr` и `WeakPtr` |
| Синхронизация | `Atomic`, `Mutex` (спин + sleep), `LockGuard` | `ConditionVariable`, `SharedMutex`, `CallOnce`, `Semaphore`, `Barrier` | высокая для Jobs |
| Строки | `Append`, `+=`, `+`, `==`, `ToString`, `FormatSigned/Unsigned/Floating` | `Find`, `Substr`, `StartsWith`, `EndsWith`, `Compare`, `Insert`, `Erase`, `ToUpper`/`ToLower`, разбор чисел, форматирование по шаблону | средняя |
| Время, потоки, файлы | пусто (раздел 4) | аналоги `chrono`, `thread`, `filesystem`, `path` | высокая |
| Ввод и вывод | вывод: `Logging` (≈`cerr`/`clog`, Warning и выше в stderr, остальное в stdout) через `System::WriteOutput` | чтение stdin (≈`cin`) в `Kernel.System` отсутствует; форматированного вывода (≈`printf`/`format`) нет | низкая (понадобится для консольных утилит `Tools.*`) |

Что неполно в уже написанном:

- `Array`: нет `Back`, `Front`, `Pop`, `Insert`, `Resize`, `Find`, удаления перестановкой.
  Есть `begin`/`end`, `operator[]`, `Add`, `RemoveAt`, `Reserve`, `Clear`.
- `HashMap`: только `ForEach` (визитор возвращает `Bool`, `false` останавливает обход), итераторов нет;
  копирование запрещено; значения и ключи должны иметь `noexcept` перемещение.
- `StringView`: нет `Find`, `Substr`, `StartsWith`, сравнения по порядку. `String(nullptr)` и `StringView(nullptr)` дают пустую строку.
- `Function`: копии делят одну и ту же вызываемую функцию (счётчик ссылок); для move-only лямбд не подходит.
- `SharedPtr`: нет `WeakPtr`, поэтому циклы ссылок не разорвать.
- `GetTypeKey`: ключ типа это хэш строки `__PRETTY_FUNCTION__`; работает только на GCC и Clang, коллизия теоретически возможна.

## 6. Порядок работ (предложение)

1. Решить, остаются ли исключения (раздел 2): от этого зависит `Result`/`Optional` и работа с ошибками во всём ядре.
2. (сделано) масштабируемость аллокатора, `Mutex`, `__PRETTY_FUNCTION__` в `Kernel.Compiler`.
3. `Span`, `Optional`, `UniquePtr`, `WeakPtr`, `ConditionVariable`: от них зависит больше всего модулей Engine.
5. Наполнить `Kernel.Time`, `Kernel.Threading`, `Kernel.FileSystem`: на них уже ссылаются
   `Engine.GameLoop`, `Engine.Jobs`, `Engine.Assets`.
