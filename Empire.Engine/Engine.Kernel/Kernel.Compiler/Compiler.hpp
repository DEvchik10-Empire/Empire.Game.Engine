#pragma once

// Единственное место в Kernel, где допускаются встроенные функции и предопределённые макросы компилятора
// (GCC и Clang). Остальные модули пользуются только этими обёртками.
//
// От Compiler зависят Kernel.Types и Kernel.Sync, поэтому здесь нельзя подключать другие модули Kernel
// и использовать KernelTypes: вместо них встроенные типы языка.

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelCompiler
                {
                    [[nodiscard]] constexpr const char* CurrentFileName(const char* File = __builtin_FILE()) noexcept
                    {
                        return File;
                    }

                    [[nodiscard]] constexpr const char* CurrentFunctionName(const char* Function = __builtin_FUNCTION()) noexcept
                    {
                        return Function;
                    }

                    [[nodiscard]] constexpr unsigned int CurrentLineNumber(
                        unsigned int Line = static_cast<unsigned int>(__builtin_LINE())
                    ) noexcept
                    {
                        return Line;
                    }

                    // Подпись функции, в которую компилятор подставил аргумент шаблона (содержит имя типа). GCC и Clang.
                    template <typename T>
                    consteval const char* TypeSignature() noexcept
                    {
                        return __PRETTY_FUNCTION__;
                    }

                    // Отладочная ли сборка. Единственное место, где это читается из препроцессора (xmake в release задаёт NDEBUG).
#ifdef NDEBUG
                    inline constexpr bool IsDebugBuild = false;
#else
                    inline constexpr bool IsDebugBuild = true;
#endif

                    // Включён ли ThreadSanitizer (он не поддерживает отдельные барьеры памяти, см. AtomicThreadFence)
#if defined(__SANITIZE_THREAD__)
                    inline constexpr bool IsThreadSanitizer = true;
#elif defined(__has_feature)
#if __has_feature(thread_sanitizer)
                    inline constexpr bool IsThreadSanitizer = true;
#else
                    inline constexpr bool IsThreadSanitizer = false;
#endif
#else
                    inline constexpr bool IsThreadSanitizer = false;
#endif

                    // Свойства типов, которые компилятор отдаёт только встроенными функциями (у Clang для деструктора
                    // есть __is_trivially_destructible, а __has_trivial_destructor он считает устаревшим; у GCC наоборот).
                    template <typename T>
                    inline constexpr bool IsTriviallyCopyable = __is_trivially_copyable(T);

#if defined(__has_builtin)
#if __has_builtin(__is_trivially_destructible)
                    template <typename T>
                    inline constexpr bool IsTriviallyDestructible = __is_trivially_destructible(T);
#else
                    template <typename T>
                    inline constexpr bool IsTriviallyDestructible = __has_trivial_destructor(T);
#endif
#else
                    template <typename T>
                    inline constexpr bool IsTriviallyDestructible = __has_trivial_destructor(T);
#endif

                    template <typename T, typename... Arguments>
                    inline constexpr bool IsTriviallyConstructible = __is_trivially_constructible(T, Arguments...);

                    // Размер строки кэша: структуры, к которым обращаются разные потоки, выравниваются по нему против ложного разделения
                    inline constexpr unsigned int CacheLineSize = 64;

                    // Число нулевых бит с младшего края. Для нуля возвращает 32 (у __builtin_ctz результат для нуля не определён).
                    [[nodiscard]] constexpr unsigned int CountTrailingZeros(unsigned int Value) noexcept
                    {
                        return Value != 0 ? static_cast<unsigned int>(__builtin_ctz(Value)) : 32u;
                    }

                    // Число нулевых бит со старшего края. Для нуля возвращает 32.
                    [[nodiscard]] constexpr unsigned int CountLeadingZeros(unsigned int Value) noexcept
                    {
                        return Value != 0 ? static_cast<unsigned int>(__builtin_clz(Value)) : 32u;
                    }

                    // Работа с группой из 16 байт, нужна открытой адресации в хеш-таблицах (по 16 корзин за раз).
                    // Читает ровно 16 байт по Bytes, выравнивание не требуется. Результат это маска из 16 бит: бит i равен 1,
                    // если условие выполнено для байта i. На x86 это одна инструкция pcmpeqb плюс pmovmskb, на остальных
                    // архитектурах пока обычный цикл (для NEON будет свой путь в Math.Simd).

                    // Какие из 16 байт равны Value
                    [[nodiscard]] inline unsigned int EqualMask16(const unsigned char* Bytes, unsigned char Value) noexcept
                    {
#if defined(__SSE2__)
                        typedef char Vector16 __attribute__((vector_size(16)));

                        Vector16 Loaded;
                        __builtin_memcpy(&Loaded, Bytes, 16);

                        Vector16 Expected;
                        for (int Index = 0; Index < 16; ++Index)
                        {
                            Expected[Index] = static_cast<char>(Value);
                        }

                        return static_cast<unsigned int>(__builtin_ia32_pmovmskb128(Loaded == Expected));
#else
                        unsigned int Mask = 0;

                        for (unsigned int Index = 0; Index < 16; ++Index)
                        {
                            Mask |= static_cast<unsigned int>(Bytes[Index] == Value) << Index;
                        }

                        return Mask;
#endif
                    }

                    // У каких из 16 байт старший бит равен 1
                    [[nodiscard]] inline unsigned int HighBitMask16(const unsigned char* Bytes) noexcept
                    {
#if defined(__SSE2__)
                        typedef char Vector16 __attribute__((vector_size(16)));

                        Vector16 Loaded;
                        __builtin_memcpy(&Loaded, Bytes, 16);

                        return static_cast<unsigned int>(__builtin_ia32_pmovmskb128(Loaded));
#else
                        unsigned int Mask = 0;

                        for (unsigned int Index = 0; Index < 16; ++Index)
                        {
                            Mask |= static_cast<unsigned int>((Bytes[Index] & 0x80) != 0) << Index;
                        }

                        return Mask;
#endif
                    }

                    // Подсказка процессору, что поток крутится в ожидании (снижает нагрузку на шину и соседний гипер-поток).
                    inline void Pause() noexcept
                    {
#if defined(__x86_64__) || defined(__i386__)
                        __builtin_ia32_pause();
#elif defined(__aarch64__) || defined(__arm__)
                        __asm__ __volatile__("yield" ::: "memory");
#else
                        __asm__ __volatile__("" ::: "memory");
#endif
                    }

                    // Немедленная аварийная остановка процесса.
                    [[noreturn]] inline void Trap() noexcept
                    {
                        __builtin_trap();
                    }

                    // Порядки памяти для встроенных атомарных операций.
                    enum class AtomicOrder : int
                    {
                        Relaxed = __ATOMIC_RELAXED,
                        Acquire = __ATOMIC_ACQUIRE,
                        Release = __ATOMIC_RELEASE,
                        AcquireRelease = __ATOMIC_ACQ_REL,
                        SequentiallyConsistent = __ATOMIC_SEQ_CST
                    };

                    // Отдельный барьер памяти
                    inline void AtomicThreadFence(AtomicOrder Order) noexcept
                    {
                        __atomic_thread_fence(static_cast<int>(Order));
                    }

                    template <typename T>
                    [[nodiscard]] T AtomicLoad(const T* Target, AtomicOrder Order) noexcept
                    {
                        return __atomic_load_n(Target, static_cast<int>(Order));
                    }

                    template <typename T>
                    void AtomicStore(T* Target, T Value, AtomicOrder Order) noexcept
                    {
                        __atomic_store_n(Target, Value, static_cast<int>(Order));
                    }

                    template <typename T>
                    T AtomicExchange(T* Target, T Value, AtomicOrder Order) noexcept
                    {
                        return __atomic_exchange_n(Target, Value, static_cast<int>(Order));
                    }

                    template <typename T>
                    bool AtomicCompareExchange(T* Target, T* Expected, T Desired, AtomicOrder Success, AtomicOrder Failure) noexcept
                    {
                        return __atomic_compare_exchange_n(
                            Target, Expected, Desired, false, static_cast<int>(Success), static_cast<int>(Failure)
                        );
                    }

                    template <typename T>
                    T AtomicFetchAdd(T* Target, T Amount, AtomicOrder Order) noexcept
                    {
                        return __atomic_fetch_add(Target, Amount, static_cast<int>(Order));
                    }

                    template <typename T>
                    T AtomicFetchSub(T* Target, T Amount, AtomicOrder Order) noexcept
                    {
                        return __atomic_fetch_sub(Target, Amount, static_cast<int>(Order));
                    }
                }
            }
        }
    }
}
