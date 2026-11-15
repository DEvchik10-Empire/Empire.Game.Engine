// Поведенческий тест Kernel.Algorithms: InsertionSort, QuickSort, BinaryHeap.
// Первая часть перенесена из тестов Jolt (InsertionSortTest, QuickSortTest, BinaryHeapTest), вторая сверяет результат со стандартной сортировкой.
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Algorithms.hpp"

#include "Empire.Kernel/Kernel.Algorithms/Algorithms.BinaryHeap.hpp"
#include "Empire.Kernel/Kernel.Algorithms/Algorithms.Compare.hpp"
#include "Empire.Kernel/Kernel.Algorithms/Algorithms.InsertionSort.hpp"
#include "Empire.Kernel/Kernel.Algorithms/Algorithms.QuickSort.hpp"
#include <algorithm>
#include <climits>
#include <cstdio>
#include <random>
#include <vector>

namespace KA = DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelAlgorithms;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)
#define SECTION(n) std::printf("[%s]\n", n)

// Общий набор проверок для любой сортировки вида Sort(Begin, End) и Sort(Begin, End, Order)
template <typename SortDefault, typename SortWithOrder>
static void RunSortTests(SortDefault Sort, SortWithOrder SortOrdered, int RandomPairs)
{
    {   // упорядоченный
        std::vector<int> Array;
        for (int I = 0; I < 100; I++) Array.push_back(I);
        Sort(Array.begin(), Array.end());
        for (int I = 0; I < 100; I++) CHECK(Array[I] == I);
    }
    {   // упорядоченный, компаратор по убыванию
        std::vector<int> Array;
        for (int I = 0; I < 100; I++) Array.push_back(I);
        SortOrdered(Array.begin(), Array.end(), KA::Greater{});
        for (int I = 0; I < 100; I++) CHECK(Array[I] == 99 - I);
    }
    {   // в обратном порядке
        std::vector<int> Array;
        for (int I = 0; I < 100; I++) Array.push_back(99 - I);
        Sort(Array.begin(), Array.end());
        for (int I = 0; I < 100; I++) CHECK(Array[I] == I);
    }
    {   // случайный: каждое значение вставлено дважды (в начало и в конец)
        std::mt19937 Random(1234);
        std::vector<unsigned> Array;
        for (int I = 0; I < RandomPairs; I++)
        {
            unsigned Value = Random();
            Array.insert(Array.begin(), Value);
            Array.push_back(Value);
        }
        Sort(Array.begin(), Array.end());
        for (size_t I = 0; I + 2 < Array.size(); I += 2)
        {
            CHECK(Array[I] == Array[I + 1]);
            CHECK(Array[I] <= Array[I + 2]);
        }
    }
    {   // пустой, один элемент, два элемента
        std::vector<int> Empty;
        Sort(Empty.begin(), Empty.end());
        CHECK(Empty.empty());

        std::vector<int> One { 1 };
        Sort(One.begin(), One.end());
        CHECK(One[0] == 1);

        std::vector<int> Two { 2, 1 };
        Sort(Two.begin(), Two.end());
        CHECK(Two[0] == 1 && Two[1] == 2);
    }
}

int RunAlgorithmsUnitTests()
{
    SECTION("InsertionSort (из Jolt)");
    RunSortTests(
        [](auto B, auto E) { KA::InsertionSort(B, E); },
        [](auto B, auto E, auto O) { KA::InsertionSort(B, E, O); },
        100);

    SECTION("QuickSort (из Jolt)");
    RunSortTests(
        [](auto B, auto E) { KA::QuickSort(B, E); },
        [](auto B, auto E, auto O) { KA::QuickSort(B, E, O); },
        1000);

    SECTION("BinaryHeap (из Jolt)");
    {
        std::vector<int> Source;
        Source.reserve(1100);
        for (int I = 0; I < 1000; ++I) Source.push_back(I);
        for (int I = 0; I < 1000; I += 10) Source.push_back(I);   // дубликаты

        std::mt19937 Random(123);
        std::shuffle(Source.begin(), Source.end(), Random);

        std::vector<int> Heap;
        for (int Value : Source)
        {
            Heap.push_back(Value);
            KA::BinaryHeapPush(Heap.begin(), Heap.end(), KA::Less{});
        }

        int Last = INT_MAX;
        int Seen[1000] = {};
        bool Duplicates = true;
        bool Descending = true;
        while (!Heap.empty())
        {
            KA::BinaryHeapPop(Heap.begin(), Heap.end(), KA::Less{});
            int Current = Heap.back();
            Duplicates &= ++Seen[Current] <= (Current % 10 == 0 ? 2 : 1);
            Heap.pop_back();
            Descending &= Current <= Last;
            Last = Current;
        }
        CHECK(Duplicates);
        CHECK(Descending);
    }

    SECTION("Сверка со стандартной сортировкой");
    {
        std::mt19937 Random(777);
        bool AllEqual = true;

        // Размеры вокруг порога 32 (переход на вставки) и крупные, значения с большим числом повторов
        for (int Size : {0, 1, 2, 3, 31, 32, 33, 34, 64, 100, 257, 1000, 4096, 20000})
        {
            for (int Range : {2, 10, 1000000})
            {
                std::vector<int> Original(Size);
                for (int& Value : Original) Value = static_cast<int>(Random() % Range);

                std::vector<int> Expected = Original;
                std::sort(Expected.begin(), Expected.end());

                std::vector<int> ByQuick = Original;
                KA::QuickSort(ByQuick.begin(), ByQuick.end());
                AllEqual &= ByQuick == Expected;

                std::vector<int> ByInsertion = Original;
                if (Size <= 1000)
                {
                    KA::InsertionSort(ByInsertion.begin(), ByInsertion.end());
                    AllEqual &= ByInsertion == Expected;
                }

                // Все равные и уже отсортированные по убыванию
                std::vector<int> Equal(Size, 5);
                KA::QuickSort(Equal.begin(), Equal.end());
                AllEqual &= std::all_of(Equal.begin(), Equal.end(), [](int V) { return V == 5; });

                std::vector<int> Descending = Expected;
                std::reverse(Descending.begin(), Descending.end());
                KA::QuickSort(Descending.begin(), Descending.end());
                AllEqual &= Descending == Expected;
            }
        }
        CHECK(AllEqual);

        // Работает на обычных указателях и в constexpr
        int Raw[] = { 5, 3, 9, 1, 7 };
        KA::QuickSort(Raw, Raw + 5);
        CHECK(Raw[0] == 1 && Raw[4] == 9);

        constexpr bool Compile = []() constexpr {
            int Values[] = { 4, 2, 8, 6 };
            KA::InsertionSort(Values, Values + 4);
            return Values[0] == 2 && Values[3] == 8;
        }();
        CHECK(Compile);
    }

    std::printf("Algorithms: %d проверок, %d ошибок\n", Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
