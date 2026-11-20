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
                    [[nodiscard]] constexpr bool IsConstantEvaluated() noexcept
                    {
                        return __builtin_is_constant_evaluated();
                    }

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

                    // Включён ли AddressSanitizer (нужен для условных диагностик и адаптации allocator paths).
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
                    inline constexpr bool IsAddressSanitizer = true;
#elif defined(__SANITIZE_ADDRESS__)
                    inline constexpr bool IsAddressSanitizer = true;
#else
                    inline constexpr bool IsAddressSanitizer = false;
#endif
#elif defined(__SANITIZE_ADDRESS__)
                    inline constexpr bool IsAddressSanitizer = true;
#else
                    inline constexpr bool IsAddressSanitizer = false;
#endif

                    // Свойства типов, которые компилятор отдаёт только встроенными функциями (у Clang для деструктора
                    // есть __is_trivially_destructible, а __has_trivial_destructor он считает устаревшим; у GCC наоборот).
                    template <typename T>
                    inline constexpr bool IsTriviallyCopyable = __is_trivially_copyable(T);

                    template <typename To, typename From>
                    [[nodiscard]] constexpr To BitCast(const From& Value) noexcept
                    {
                        static_assert(sizeof(To) == sizeof(From));
                        static_assert(IsTriviallyCopyable<To> && IsTriviallyCopyable<From>);
                        return __builtin_bit_cast(To, Value);
                    }

                    template <typename T>
                    [[nodiscard]] inline T LoadUnaligned(const void* Address) noexcept
                    {
                        static_assert(IsTriviallyCopyable<T>);
                        T Value;
                        __builtin_memcpy(&Value, Address, sizeof(T));
                        return Value;
                    }

                    [[nodiscard]] inline float SquareRoot(float Value) noexcept
                    {
                        return __builtin_sqrtf(Value);
                    }

                    [[nodiscard]] inline double SquareRoot(double Value) noexcept
                    {
                        return __builtin_sqrt(Value);
                    }

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

                    // Архитектура CPU нужна только для выбора реализаций внутри compiler/platform boundaries.
#if defined(__x86_64__) || defined(__i386__)
                    inline constexpr bool IsX86Architecture = true;
#else
                    inline constexpr bool IsX86Architecture = false;
#endif

#if defined(__aarch64__) || defined(__arm__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                    inline constexpr bool IsArmArchitecture = true;
#else
                    inline constexpr bool IsArmArchitecture = false;
#endif

#if defined(__riscv)
                    inline constexpr bool IsRISCVArchitecture = true;
#else
                    inline constexpr bool IsRISCVArchitecture = false;
#endif

                    // Размер строки кэша: структуры, к которым обращаются разные потоки, выравниваются по нему против ложного разделения
                    inline constexpr unsigned int CacheLineSize = 64;

#if defined(__x86_64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(__LP64__) || (defined(__riscv_xlen) && __riscv_xlen == 64)
                    inline constexpr unsigned int CpuAddressBits = 64;
#else
                    inline constexpr unsigned int CpuAddressBits = 32;
#endif

                    // Jolt's VectorAlignment stays 16 bytes on x86/ARM64 even when AVX is available.
#if defined(__arm__) && !defined(__aarch64__)
                    inline constexpr unsigned int VectorAlignment = 8;
                    inline constexpr unsigned int DoubleVectorAlignment = 8;
#elif defined(__x86_64__) || defined(__i386__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC) || (defined(__riscv_xlen) && __riscv_xlen == 64)
                    inline constexpr unsigned int VectorAlignment = 16;
                    inline constexpr unsigned int DoubleVectorAlignment = 32;
#else
                    inline constexpr unsigned int VectorAlignment = 16;
                    inline constexpr unsigned int DoubleVectorAlignment = 8;
#endif

#if defined(__SSE2__) || defined(__x86_64__)
                    inline constexpr bool SupportsSSE2 = true;
#else
                    inline constexpr bool SupportsSSE2 = false;
#endif

#if defined(__SSE4_1__) || defined(__SSE4_2__) || defined(__AVX__) || defined(__AVX2__)
                    inline constexpr bool SupportsSSE41 = true;
#else
                    inline constexpr bool SupportsSSE41 = false;
#endif

#if defined(__SSE4_2__) || defined(__AVX__) || defined(__AVX2__)
                    inline constexpr bool SupportsSSE42 = true;
#else
                    inline constexpr bool SupportsSSE42 = false;
#endif

#if defined(__AVX__) || defined(__AVX2__)
                    inline constexpr bool SupportsAVX = true;
#else
                    inline constexpr bool SupportsAVX = false;
#endif

#if defined(__AVX2__) || (defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512DQ__))
                    inline constexpr bool SupportsAVX2 = true;
#else
                    inline constexpr bool SupportsAVX2 = false;
#endif

#if defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512DQ__)
                    inline constexpr bool SupportsAVX512 = true;
#else
                    inline constexpr bool SupportsAVX512 = false;
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                    inline constexpr bool SupportsNEON = true;
#else
                    inline constexpr bool SupportsNEON = false;
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                    inline constexpr bool SupportsARM64NEON = true;
#else
                    inline constexpr bool SupportsARM64NEON = false;
#endif

#if defined(__riscv_vector)
                    inline constexpr bool SupportsRISCVVector = true;
#else
                    inline constexpr bool SupportsRISCVVector = false;
#endif

#if defined(__F16C__) || defined(__AVX2__)
                    inline constexpr bool SupportsF16C = true;
#else
                    inline constexpr bool SupportsF16C = false;
#endif

#if defined(__LZCNT__) || defined(__AVX2__)
                    inline constexpr bool SupportsLZCNT = true;
#else
                    inline constexpr bool SupportsLZCNT = false;
#endif

#if defined(__BMI__) || defined(__AVX2__)
                    inline constexpr bool SupportsTZCNT = true;
#else
                    inline constexpr bool SupportsTZCNT = false;
#endif

#if defined(__wasm_simd128__)
                    inline constexpr bool SupportsWasmSimd128 = true;
#else
                    inline constexpr bool SupportsWasmSimd128 = false;
#endif

#if defined(__FMA__) || defined(__ARM_FEATURE_FMA)
                    inline constexpr bool SupportsFMA = true;
#else
                    inline constexpr bool SupportsFMA = false;
#endif

                    // Подсказка процессору о скором чтении адреса (перенесено из Jolt 5.7.0, Core/Prefetch.h).
                    inline void PrefetchRead(const void* Address) noexcept
                    {
                        __builtin_prefetch(Address, 0, 3);
                    }

                    // Эквивалент Jolt PrefetchL1: подсказка о скором чтении в L1 cache.
                    inline void PrefetchL1(const void* Address) noexcept
                    {
                        PrefetchRead(Address);
                    }

                    // Подсказка процессору о скорой записи по адресу.
                    inline void PrefetchWrite(const void* Address) noexcept
                    {
                        __builtin_prefetch(Address, 1, 3);
                    }

                    // Чтение регистра управления FP на поддерживаемых CPU (перенесено из Jolt 5.7.0, Core/FPControlWord.h).
                    [[nodiscard]] inline unsigned long long ReadFPControlWord() noexcept
                    {
#if defined(__x86_64__) || defined(__i386__)
                        unsigned int Value;
                        __asm__ __volatile__("stmxcsr %0" : "=m"(Value));
                        return Value;
#elif defined(__aarch64__)
                        unsigned long long Value;
                        __asm__ __volatile__("mrs %0, fpcr" : "=r"(Value));
                        return Value;
#elif defined(__arm__)
                        unsigned int Value;
                        __asm__ __volatile__("vmrs %0, fpscr" : "=r"(Value));
                        return Value;
#else
                        return 0;
#endif
                    }

                    inline void WriteFPControlWord(unsigned long long Value) noexcept
                    {
#if defined(__x86_64__) || defined(__i386__)
                        const unsigned int Control = static_cast<unsigned int>(Value);
                        __asm__ __volatile__("ldmxcsr %0" : : "m"(Control));
#elif defined(__aarch64__)
                        __asm__ __volatile__("msr fpcr, %0" : : "r"(Value));
#elif defined(__arm__)
                        const unsigned int Control = static_cast<unsigned int>(Value);
                        __asm__ __volatile__("vmsr fpscr, %0" : : "r"(Control));
#else
                        static_cast<void>(Value);
#endif
                    }

                    // Счётчик циклов процессора для приблизительных измерений профайлера (перенесено из Jolt 5.7.0, Core/TickCounter.h).
                    [[nodiscard]] inline unsigned long long ReadProcessorTickCount() noexcept
                    {
#if defined(__x86_64__) || defined(__i386__)
                        return __builtin_ia32_rdtsc();
#elif defined(__aarch64__)
                        unsigned long long Value;
                        __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(Value));
                        return Value;
#else
                        return 0;
#endif
                    }

                    // RAII-маска регистра FP (перенесено из Jolt 5.7.0, Core/FPControlWord.h).
                    template <unsigned long long Value, unsigned long long Mask>
                    class FPControlWord
                    {
                        private:
                            unsigned long long Previous = 0;

                        public:
                            FPControlWord() noexcept
                                : Previous(ReadFPControlWord())
                            {
                                WriteFPControlWord((Previous & ~Mask) | Value);
                            }

                            ~FPControlWord() noexcept
                            {
                                const unsigned long long Current = ReadFPControlWord();
                                WriteFPControlWord((Current & ~Mask) | (Previous & Mask));
                            }

                            FPControlWord(const FPControlWord&) = delete;
                            FPControlWord& operator=(const FPControlWord&) = delete;
                    };

                    // Денормалы обнуляются как в Jolt 5.7.0, Core/FPFlushDenormals.h: FTZ в MXCSR на x86 и FZ в FPCR/FPSCR на ARM.
#if defined(__x86_64__) || defined(__i386__)
                    inline constexpr unsigned long long FlushDenormalsMask = 1ull << 15;
#elif defined(__aarch64__) || defined(__arm__)
                    inline constexpr unsigned long long FlushDenormalsMask = 1ull << 24;
#else
                    inline constexpr unsigned long long FlushDenormalsMask = 0;
#endif

                    using FPFlushDenormals = FPControlWord<FlushDenormalsMask, FlushDenormalsMask>;

                    // Маски исключений из Jolt 5.7.0, Core/FPException.h.
#if defined(__x86_64__) || defined(__i386__)
                    inline constexpr unsigned long long FPExceptionEnableMask = (1ull << 7) | (1ull << 9) | (1ull << 10);
                    inline constexpr unsigned long long FPExceptionInvalidMask = 1ull << 7;
                    inline constexpr unsigned long long FPExceptionDivideByZeroMask = 1ull << 9;
                    inline constexpr unsigned long long FPExceptionOverflowMask = 1ull << 10;
#elif defined(__aarch64__) || defined(__arm__)
                    inline constexpr unsigned long long FPExceptionEnableMask = (1ull << 8) | (1ull << 9) | (1ull << 10);
                    inline constexpr unsigned long long FPExceptionInvalidMask = 1ull << 8;
                    inline constexpr unsigned long long FPExceptionDivideByZeroMask = 1ull << 9;
                    inline constexpr unsigned long long FPExceptionOverflowMask = 1ull << 10;
#else
                    inline constexpr unsigned long long FPExceptionEnableMask = 0;
                    inline constexpr unsigned long long FPExceptionInvalidMask = 0;
                    inline constexpr unsigned long long FPExceptionDivideByZeroMask = 0;
                    inline constexpr unsigned long long FPExceptionOverflowMask = 0;
#endif

#if defined(__x86_64__) || defined(__i386__)
                    inline constexpr unsigned long long FPExceptionEnableValue = 0;
                    inline constexpr unsigned long long FPExceptionDisableValue = FPExceptionEnableMask;
#elif defined(__aarch64__) || defined(__arm__)
                    inline constexpr unsigned long long FPExceptionEnableValue = FPExceptionEnableMask;
                    inline constexpr unsigned long long FPExceptionDisableValue = 0;
#else
                    inline constexpr unsigned long long FPExceptionEnableValue = 0;
                    inline constexpr unsigned long long FPExceptionDisableValue = 0;
#endif

                    using FPExceptionsEnable = FPControlWord<FPExceptionEnableValue, FPExceptionEnableMask>;
                    using FPExceptionDisableInvalid = FPControlWord<FPExceptionDisableValue, FPExceptionInvalidMask>;
                    using FPExceptionDisableDivByZero = FPControlWord<FPExceptionDisableValue, FPExceptionDivideByZeroMask>;
                    using FPExceptionDisableOverflow = FPControlWord<FPExceptionDisableValue, FPExceptionOverflowMask>;

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
                        using Vector16 = char __attribute__((vector_size(16)));

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
                        using Vector16 = char __attribute__((vector_size(16)));

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

                    // Остановить выполнение в отладчике, если архитектура поддерживает breakpoint instruction.
                    inline void Breakpoint() noexcept
                    {
#if defined(__x86_64__) || defined(__i386__)
                        __asm__ __volatile__("int $0x3");
#elif defined(__aarch64__)
                        __asm__ __volatile__("brk #0");
#elif defined(__arm__)
                        __asm__ __volatile__("bkpt #0");
#elif defined(__wasm__)
                        // WebAssembly не предоставляет переносимой инструкции debugger breakpoint.
#else
                        __builtin_trap();
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

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.Simd.Neon.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.Simd.RISCV.hpp"
