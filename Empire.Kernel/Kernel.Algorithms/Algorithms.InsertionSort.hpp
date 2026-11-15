#pragma once

#include "Empire.Kernel/Kernel.Algorithms/Algorithms.Compare.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelAlgorithms
            {
                // Сортировка вставками. Устойчивая, быстрая на коротких диапазонах (QuickSort переключается на неё).
                // Нужны итераторы произвольного доступа.
                template <typename Iterator, typename Compare>
                constexpr void InsertionSort(Iterator Begin, Iterator End, Compare Order)
                {
                    // Пустой диапазон сортировать не нужно
                    if (Begin == End)
                    {
                        return;
                    }

                    // Начинаем со второго элемента
                    for (Iterator Current = Begin + 1; Current != End; ++Current)
                    {
                        // Выносим элемент во временное значение
                        auto Value = KernelTypes::Move(*Current);

                        // Элемент идёт перед Begin (отдельная ветка, потому что итератор нельзя уменьшить ниже Begin)
                        if (Order(Value, *Begin))
                        {
                            // Сдвигаем всё вправо, освобождая место под Value
                            Iterator Previous;

                            for (Iterator Position = Current; Position != Begin; Position = Previous)
                            {
                                Previous = Position - 1;
                                *Position = KernelTypes::Move(*Previous);
                            }

                            *Begin = KernelTypes::Move(Value);
                        }
                        else
                        {
                            // Сдвигаем вправо, пока элементы больше Value
                            Iterator Position = Current;

                            for (Iterator Previous = Position - 1; Order(Value, *Previous); Position = Previous, --Previous)
                            {
                                *Position = KernelTypes::Move(*Previous);
                            }

                            *Position = KernelTypes::Move(Value);
                        }
                    }
                }

                template <typename Iterator>
                constexpr void InsertionSort(Iterator Begin, Iterator End)
                {
                    InsertionSort(Begin, End, Less{});
                }
            }
        }
    }
}
