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
        namespace EmpireKernel
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
