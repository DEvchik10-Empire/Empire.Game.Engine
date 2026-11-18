#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Algorithms/Algorithms.Compare.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastHashTable.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    // Вспомогательный класс: достаёт ключ из пары для FastHashMap
                    template <typename Key, typename Value>
                    class FastHashMapDetail
                    {
                        public:
                            static const Key& GetKey(const KernelTypes::Pair<Key, Value>& KeyValue)
                            {
                                return KeyValue.First;
                            }
                    };

                    // Хеш-карта на открытой адресации (перенесена из Jolt, Core/UnorderedMap.h).
                    // Хеш-функция должна возвращать 64 бита. Существующий HashMap этого модуля не заменяется, это отдельный контейнер.
                    // Порядок обхода не определён. Указатели и итераторы недействительны после вставки, удаления и роста.
                    template <typename Key, typename Value, typename HashType = Hash<Key>, typename KeyEqual = KernelAlgorithms::EqualTo>
                    class FastHashMap : public FastHashTable<Key, KernelTypes::Pair<Key, Value>, FastHashMapDetail<Key, Value>, HashType, KeyEqual>
                    {
                        using Base = FastHashTable<Key, KernelTypes::Pair<Key, Value>, FastHashMapDetail<Key, Value>, HashType, KeyEqual>;

                        public:
                            using SizeType = typename Base::SizeType;
                            using Iterator = typename Base::Iterator;
                            using ConstIterator = typename Base::ConstIterator;
                            using ValueType = typename Base::ValueType;

                            // Значение по ключу. Если ключа нет, добавляется Value().
                            Value& operator[](const Key& KeyToFind)
                            {
                                SizeType Index = 0;
                                KernelTypes::Bool Inserted = this->InsertKey(KeyToFind, Index);

                                ValueType& KeyValue = this->GetElement(Index);
                                if (Inserted)
                                {
                                    KernelTypes::ConstructAt(&KeyValue, KeyToFind, Value());
                                }

                                return KeyValue.Second;
                            }

                            // Добавить элемент, построив значение из аргументов, если ключа ещё нет
                            template <typename... Arguments>
                            KernelTypes::Pair<Iterator, KernelTypes::Bool> TryEmplace(const Key& KeyToInsert, Arguments&&... Args)
                            {
                                SizeType Index = 0;
                                KernelTypes::Bool Inserted = this->InsertKey(KeyToInsert, Index);

                                if (Inserted)
                                {
                                    KernelTypes::ConstructAt(&this->GetElement(Index), KernelTypes::SecondFromArgumentsTag{}, KeyToInsert, KernelTypes::Forward<Arguments>(Args)...);
                                }

                                return KernelTypes::Pair<Iterator, KernelTypes::Bool>(Iterator(this, Index), Inserted);
                            }

                            template <typename... Arguments>
                            KernelTypes::Pair<Iterator, KernelTypes::Bool> TryEmplace(Key&& KeyToInsert, Arguments&&... Args)
                            {
                                SizeType Index = 0;
                                KernelTypes::Bool Inserted = this->InsertKey(KeyToInsert, Index);

                                if (Inserted)
                                {
                                    KernelTypes::ConstructAt(&this->GetElement(Index), KernelTypes::SecondFromArgumentsTag{}, KernelTypes::Move(KeyToInsert), KernelTypes::Forward<Arguments>(Args)...);
                                }

                                return KernelTypes::Pair<Iterator, KernelTypes::Bool>(Iterator(this, Index), Inserted);
                            }

                            // Версия Find для чтения
                            using Base::Find;

                            // Версия Find, возвращающая изменяемый итератор
                            Iterator Find(const Key& KeyToFind)
                            {
                                ConstIterator Found = Base::Find(KeyToFind);
                                return Iterator(this, Found.Index);
                            }
                    };
                }
            }
        }
    }
}
