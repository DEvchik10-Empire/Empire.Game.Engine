#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Placement.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelTypes
            {
                template <typename T>
                struct RemoveReferenceTrait
                {
                    using Type = T;
                };

                template <typename T>
                struct RemoveReferenceTrait<T&>
                {
                    using Type = T;
                };

                template <typename T>
                struct RemoveReferenceTrait<T&&>
                {
                    using Type = T;
                };

                template <typename T>
                using RemoveReference = typename RemoveReferenceTrait<T>::Type;

                template <typename T>
                struct RemoveConstTrait
                {
                    using Type = T;
                };

                template <typename T>
                struct RemoveConstTrait<const T>
                {
                    using Type = T;
                };

                template <typename T>
                struct RemoveVolatileTrait
                {
                    using Type = T;
                };

                template <typename T>
                struct RemoveVolatileTrait<volatile T>
                {
                    using Type = T;
                };

                template <typename T>
                using RemoveCV = typename RemoveVolatileTrait<typename RemoveConstTrait<T>::Type>::Type;

                template <typename T>
                using RemoveCVReference = RemoveCV<RemoveReference<T>>;

                template <typename T>
                T&& Declval() noexcept;

                template <typename T, typename U>
                inline constexpr Bool IsSame = false;

                template <typename T>
                inline constexpr Bool IsSame<T, T> = true;

                template <typename T, typename... Candidates>
                inline constexpr Bool IsAnyOf = (IsSame<T, Candidates> || ...);

                template <typename T>
                inline constexpr Bool IsVoid = IsSame<RemoveCV<T>, void>;

                template <typename T>
                inline constexpr Bool IsIntegral = IsAnyOf<
                    RemoveCV<T>,
                    bool, char, signed char, unsigned char,
                    short, unsigned short,
                    int, unsigned int,
                    long, unsigned long,
                    long long, unsigned long long,
                    wchar_t, char8_t, char16_t, char32_t
                >;

                template <typename T>
                inline constexpr Bool IsFloatingPoint = IsAnyOf<RemoveCV<T>, float, double, long double>;

                template <typename T>
                concept Integral = IsIntegral<T>;

                template <typename T>
                concept FloatingPoint = IsFloatingPoint<T>;

                template <typename T>
                concept Arithmetic = IsIntegral<T> || IsFloatingPoint<T>;

                template <Arithmetic T>
                inline constexpr Bool IsSigned = static_cast<RemoveCV<T>>(-1) < static_cast<RemoveCV<T>>(0);

                template <typename T>
                inline constexpr Bool IsCopyConstructible = requires(const T& Value) { T(Value); };

                template <typename T>
                inline constexpr Bool IsNothrowMoveConstructible = noexcept(T(Declval<T>()));

                template <typename T>
                constexpr RemoveReference<T>&& Move(T&& Value) noexcept
                {
                    return static_cast<RemoveReference<T>&&>(Value);
                }

                template <typename T>
                constexpr T&& Forward(RemoveReference<T>& Value) noexcept
                {
                    return static_cast<T&&>(Value);
                }

                template <typename T>
                constexpr T&& Forward(RemoveReference<T>&& Value) noexcept
                {
                    return static_cast<T&&>(Value);
                }

                template <typename T>
                constexpr decltype(auto) MoveIfNoexcept(T& Value) noexcept
                {
                    if constexpr (!IsNothrowMoveConstructible<T> && IsCopyConstructible<T>)
                        return static_cast<const T&>(Value);
                    else
                        return static_cast<T&&>(Value);
                }

                template <typename T>
                constexpr void Swap(T& Left, T& Right) noexcept(IsNothrowMoveConstructible<T>)
                {
                    T Temporary(Move(Left));
                    Left = Move(Right);
                    Right = Move(Temporary);
                }

                template <typename T, typename... Arguments>
                T* ConstructAt(T* Location, Arguments&&... Args)
                {
                    return ::new (PlacementTag{}, static_cast<void*>(Location)) T(Forward<Arguments>(Args)...);
                }

                template <typename T>
                void DestroyAt(T* Location) noexcept
                {
                    Location->~T();
                }

                template <typename T>
                constexpr const T& Min(const T& Left, const T& Right) noexcept
                {
                    return Right < Left ? Right : Left;
                }

                template <typename T>
                constexpr const T& Max(const T& Left, const T& Right) noexcept
                {
                    return Left < Right ? Right : Left;
                }

                template <Integral T>
                constexpr T MaxValue() noexcept
                {
                    if constexpr (IsSame<RemoveCV<T>, Bool>)
                        return true;
                    else if constexpr (IsSigned<T>)
                        return static_cast<T>((static_cast<UInt64>(1) << (sizeof(T) * 8 - 1)) - 1);
                    else
                        return static_cast<T>(~static_cast<T>(0));
                }

                using TypeKey = UInt64;

                constexpr UInt64 HashText(const Char* Text) noexcept
                {
                    UInt64 Hash = 14695981039346656037ULL;

                    for (Size Index = 0; Text[Index] != '\0'; Index++)
                    {
                        Hash ^= static_cast<UInt64>(static_cast<UInt8>(Text[Index]));
                        Hash *= 1099511628211ULL;
                    }

                    return Hash;
                }

                // Ключ типа без RTTI: хэш подписи функции, в которую компилятор подставляет аргумент шаблона.
                // Одинаков во всех .so, в отличие от адреса статической переменной. Поддерживаются GCC и Clang.
                template <typename T>
                consteval TypeKey GetTypeKey() noexcept
                {
                    return HashText(KernelCompiler::TypeSignature<T>());
                }
            }
        }
    }
}
