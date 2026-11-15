#pragma once

#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelTypes
            {
                // Метка: второй элемент строится из набора аргументов, а первый из ключа (нужна TryEmplace у хеш-таблиц)
                struct SecondFromArgumentsTag
                {
                };

                // Пара значений. В Jolt и std это first и second, здесь First и Second.
                template <typename FirstType, typename SecondType>
                struct Pair
                {
                    FirstType First;
                    SecondType Second;

                    Pair() = default;

                    Pair(const FirstType& FirstValue, const SecondType& SecondValue)
                        : First(FirstValue), Second(SecondValue)
                    {
                    }

                    template <typename OtherFirst, typename OtherSecond>
                    Pair(OtherFirst&& FirstValue, OtherSecond&& SecondValue)
                        : First(Forward<OtherFirst>(FirstValue)), Second(Forward<OtherSecond>(SecondValue))
                    {
                    }

                    template <typename KeyType, typename... Arguments>
                    Pair(SecondFromArgumentsTag, KeyType&& KeyValue, Arguments&&... Args)
                        : First(Forward<KeyType>(KeyValue)), Second(Forward<Arguments>(Args)...)
                    {
                    }

                    Pair(const Pair&) = default;
                    Pair(Pair&&) = default;
                    Pair& operator=(const Pair&) = default;
                    Pair& operator=(Pair&&) = default;
                };

                template <typename FirstType, typename SecondType>
                [[nodiscard]] constexpr Pair<RemoveCVReference<FirstType>, RemoveCVReference<SecondType>> MakePair(FirstType&& First, SecondType&& Second)
                {
                    return Pair<RemoveCVReference<FirstType>, RemoveCVReference<SecondType>>(Forward<FirstType>(First), Forward<SecondType>(Second));
                }
            }
        }
    }
}
