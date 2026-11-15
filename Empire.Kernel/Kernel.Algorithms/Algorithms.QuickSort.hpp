#pragma once

#include "Empire.Kernel/Kernel.Algorithms/Algorithms.Compare.hpp"
#include "Empire.Kernel/Kernel.Algorithms/Algorithms.InsertionSort.hpp"
#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelAlgorithms
            {
                // Помощник QuickSort: ставит медиану трёх элементов в Middle.
                template <typename Iterator, typename Compare>
                constexpr void QuickSortMedianOfThree(Iterator First, Iterator Middle, Iterator Last, Compare Order)
                {
                    // Гарантировано, потому что на 32 элементах и меньше QuickSort переходит на сортировку вставками
                    KernelAssert::Check(First != Middle && Middle != Last, "QuickSortMedianOfThree: диапазон слишком короткий");

                    if (Order(*Middle, *First))
                    {
                        KernelTypes::Swap(*First, *Middle);
                    }

                    if (Order(*Last, *First))
                    {
                        KernelTypes::Swap(*First, *Last);
                    }

                    if (Order(*Last, *Middle))
                    {
                        KernelTypes::Swap(*Middle, *Last);
                    }
                }

                // Помощник QuickSort: медиана из девяти точек (метод Ninther), результат оказывается в Middle.
                template <typename Iterator, typename Compare>
                constexpr void QuickSortNinther(Iterator First, Iterator Middle, Iterator Last, Compare Order)
                {
                    // Делим диапазон на 8 равных частей (получается 9 точек)
                    auto Step = (Last - First) >> 3;
                    auto DoubleStep = Step << 1;

                    // Медиана первых трёх точек
                    Iterator FirstMiddle = First + Step;
                    QuickSortMedianOfThree(First, FirstMiddle, First + DoubleStep, Order);

                    // Медиана вторых трёх точек
                    QuickSortMedianOfThree(Middle - Step, Middle, Middle + Step, Order);

                    // Медиана третьих трёх точек
                    Iterator LastMiddle = Last - Step;
                    QuickSortMedianOfThree(Last - DoubleStep, LastMiddle, Last, Order);

                    // Медиана из трёх медиан
                    QuickSortMedianOfThree(FirstMiddle, Middle, LastMiddle, Order);
                }

                // Быстрая сортировка. Своя, потому что результат std::sort не одинаков на разных платформах,
                // а для детерминированной симуляции порядок элементов должен совпадать везде.
                // Основа: схема разбиения Хоара, https://en.wikipedia.org/wiki/Quicksort
                template <typename Iterator, typename Compare>
                constexpr void QuickSort(Iterator Begin, Iterator End, Compare Order)
                {
                    // Цикл нужен, чтобы вызывать рекурсию один раз вместо двух
                    for (;;)
                    {
                        // Меньше двух элементов: сортировать нечего
                        auto Count = End - Begin;

                        if (Count < 2)
                        {
                            return;
                        }

                        // Мало элементов: сортировка вставками быстрее
                        if (Count <= 32)
                        {
                            InsertionSort(Begin, End, Order);
                            return;
                        }

                        // Выбираем опорный элемент
                        Iterator PivotIterator = Begin + ((Count - 1) >> 1);
                        QuickSortNinther(Begin, PivotIterator, End - 1, Order);
                        auto Pivot = *PivotIterator;

                        // Левый и правый итераторы
                        Iterator Left = Begin;
                        Iterator Right = End;

                        for (;;)
                        {
                            // Первый элемент, который больше опорного
                            while (Order(*Left, Pivot))
                            {
                                ++Left;
                            }

                            // Последний элемент, который меньше опорного
                            do
                            {
                                --Right;
                            }
                            while (Order(Pivot, *Right));

                            // Итераторы разошлись: разбиение готово
                            if (Left >= Right)
                            {
                                break;
                            }

                            KernelTypes::Swap(*Left, *Right);

                            // Первый while в идеале должен быть do { ++Left } while (...), но итератор нельзя
                            // уменьшить ниже Begin, поэтому первый шаг вынесен сюда.
                            ++Left;
                        }

                        // Средний элемент относится к левой части
                        ++Right;

                        // Рекурсия идёт в меньшую часть, а большая обрабатывается следующим витком цикла
                        if (Right - Begin < End - Right)
                        {
                            QuickSort(Begin, Right, Order);
                            Begin = Right;
                        }
                        else
                        {
                            QuickSort(Right, End, Order);
                            End = Right;
                        }
                    }
                }

                template <typename Iterator>
                constexpr void QuickSort(Iterator Begin, Iterator End)
                {
                    QuickSort(Begin, End, Less{});
                }
            }
        }
    }
}
