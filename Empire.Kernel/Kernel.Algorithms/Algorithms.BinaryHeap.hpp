#pragma once

#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelAlgorithms
            {
                // Двоичная куча по максимуму. Работает с итераторами произвольного доступа.
                // IsLessOrEqual возвращает true, если первый элемент меньше или равен второму.
                // https://en.wikipedia.org/wiki/Binary_heap

                // Добавляет в кучу элемент End - 1. До вызова [Begin, End - 1) уже куча, после вызова кучей будет [Begin, End).
                template <typename Iterator, typename Predicate>
                constexpr void BinaryHeapPush(Iterator Begin, Iterator End, Predicate IsLessOrEqual)
                {
                    // Новый размер кучи
                    auto Count = End - Begin;

                    // Идём от последнего элемента вверх
                    auto Current = Count - 1;

                    while (Current > 0)
                    {
                        auto& CurrentElement = *(Begin + Current);

                        auto Parent = (Current - 1) >> 1;
                        auto& ParentElement = *(Begin + Parent);

                        // Родитель должен быть не меньше потомка
                        if (!IsLessOrEqual(ParentElement, CurrentElement))
                        {
                            break;
                        }

                        KernelTypes::Swap(ParentElement, CurrentElement);
                        Current = Parent;
                    }
                }

                // Убирает из кучи наибольший элемент. До вызова [Begin, End) куча, после вызова кучей будет [Begin, End - 1),
                // а убранный элемент лежит в End - 1.
                template <typename Iterator, typename Predicate>
                constexpr void BinaryHeapPop(Iterator Begin, Iterator End, Predicate IsLessOrEqual)
                {
                    // Сначала переносим наибольший элемент в конец: это и есть убранный
                    KernelTypes::Swap(*(End - 1), *Begin);

                    // Новый размер кучи
                    auto Count = (End - Begin) - 1;

                    // Спускаемся от корня
                    decltype(Count) Largest = 0;

                    for (;;)
                    {
                        // Первый потомок
                        auto Child = (Largest << 1) + 1;

                        // Вышли за конец кучи: второй потомок тоже за концом
                        if (Child >= Count)
                        {
                            break;
                        }

                        auto PreviousLargest = Largest;

                        // Первый потомок больше: выбираем его
                        if (IsLessOrEqual(*(Begin + Largest), *(Begin + Child)))
                        {
                            Largest = Child;
                        }

                        // Второй потомок
                        ++Child;

                        if (Child < Count && IsLessOrEqual(*(Begin + Largest), *(Begin + Child)))
                        {
                            Largest = Child;
                        }

                        // Ничего не изменилось: куча восстановлена
                        if (PreviousLargest == Largest)
                        {
                            break;
                        }

                        KernelTypes::Swap(*(Begin + PreviousLargest), *(Begin + Largest));
                    }
                }
            }
        }
    }
}
