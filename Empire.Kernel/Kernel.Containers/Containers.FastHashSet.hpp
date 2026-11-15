#pragma once

#include "Empire.Kernel/Kernel.Algorithms/Algorithms.Compare.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.FastHashTable.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                // Вспомогательный класс: ключ множества это сам элемент
                template <typename Key>
                class FastHashSetDetail
                {
                    public:
                        static const Key& GetKey(const Key& Value)
                        {
                            return Value;
                        }
                };

                // Хеш-множество на открытой адресации (перенесено из Jolt, Core/UnorderedSet.h).
                // Хеш-функция должна возвращать 64 бита. Порядок обхода не определён.
                template <typename Key, typename HashType = Hash<Key>, typename KeyEqual = KernelAlgorithms::EqualTo>
                class FastHashSet : public FastHashTable<Key, Key, FastHashSetDetail<Key>, HashType, KeyEqual>
                {
                };
            }
        }
    }
}
