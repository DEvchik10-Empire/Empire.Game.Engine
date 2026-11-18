// Поведенческий тест контейнеров, перенесённых из Jolt: хеш-функции (Containers.Hash) и StaticArray.
// Хеши проверяются теми же значениями, что в HashCombineTest Jolt. Для StaticArray в Jolt своего теста нет,
// поэтому случаи написаны по образцу ArrayTest: подсчёт конструкторов, копий, перемещений и деструкторов.
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Containers.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"
#include <cstdio>

namespace KernelTypes = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
namespace KernelContainers = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
using KernelContainers::FastArray;
using KernelContainers::Hash;
using KernelContainers::StaticArray;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

namespace
{
    class NonTriv
    {
        public:
            NonTriv() : Number(0) { ++Constructors; }
            explicit NonTriv(int Value) : Number(Value) { ++Constructors; }
            NonTriv(const NonTriv& Other) : Number(Other.Number) { ++CopyConstructors; }
            NonTriv(NonTriv&& Other) : Number(Other.Number) { Other.Number = 0; ++MoveConstructors; }
            ~NonTriv() { ++Destructors; }

            NonTriv& operator=(const NonTriv& Other) { Number = Other.Number; return *this; }
            bool operator==(const NonTriv& Other) const { return Number == Other.Number; }
            bool operator!=(const NonTriv& Other) const { return Number != Other.Number; }

            int Value() const { return Number; }
            KernelTypes::UInt64 GetHash() const { return static_cast<KernelTypes::UInt64>(Number) * 31 + 7; }

            static void Reset() { Constructors = 0; CopyConstructors = 0; MoveConstructors = 0; Destructors = 0; }

            static inline int Constructors = 0;
            static inline int CopyConstructors = 0;
            static inline int MoveConstructors = 0;
            static inline int Destructors = 0;

            int Number;
    };

    struct Pair
    {
        int First;
        float Second;
    };

    void TestHashBytesAndString()
    {
        CHECK(KernelContainers::HashBytes("This is a test", 14) == 2733878766136413408UL);
        CHECK(KernelContainers::HashString("This is a test") == 2733878766136413408UL);

        // HashString работает на этапе компиляции
        static_assert(KernelContainers::HashString("This is a test") == 2733878766136413408UL);
        CHECK(KernelContainers::HashString("") == 0xcbf29ce484222325ULL);

        // Продолжение цепочки: хеш по частям равен хешу целого
        const KernelTypes::UInt64 Part = KernelContainers::HashBytes("This is", 7);
        CHECK(KernelContainers::HashBytes(" a test", 7, Part) == 2733878766136413408UL);
    }

    void TestHash64()
    {
        CHECK(KernelContainers::Hash64(0) != KernelContainers::Hash64(1));
        CHECK(KernelContainers::Hash64(12345) == KernelContainers::Hash64(12345));
        static_assert(KernelContainers::Hash64(1) == KernelContainers::Hash64(1));
    }

    void TestHashCombine()
    {
        int First = 0;
        KernelTypes::UInt64 FirstHash = Hash<int>{}(First);
        int Second = 1;
        KernelTypes::UInt64 SecondHash = Hash<int>{}(Second);

        // Не коммутативна
        KernelTypes::UInt64 SeedOne = FirstHash;
        KernelContainers::HashCombine(SeedOne, Second);
        KernelTypes::UInt64 SeedTwo = SecondHash;
        KernelContainers::HashCombine(SeedTwo, First);
        CHECK(SeedOne != SeedTwo);

        // Добавление нуля меняет хеш
        KernelTypes::UInt64 SeedThree = FirstHash;
        KernelContainers::HashCombine(SeedThree, First);
        CHECK(SeedThree != FirstHash);

        // HashCombineArgs совпадает с ручной цепочкой
        KernelTypes::UInt64 Manual = Hash<int>{}(7);
        KernelContainers::HashCombine(Manual, 8);
        KernelContainers::HashCombine(Manual, 9);
        CHECK(KernelContainers::HashCombineArgs(7, 8, 9) == Manual);
        CHECK(KernelContainers::HashCombineArgs(7) == Hash<int>{}(7));
    }

    void TestHashFloat()
    {
        // Минус ноль равен нулю, значит и хеши равны
        CHECK(Hash<float>{}(0.0f) == Hash<float>{}(-0.0f));
        CHECK(Hash<double>{}(0.0) == Hash<double>{}(-0.0));
        CHECK(Hash<float>{}(1.0f) != Hash<float>{}(2.0f));
        CHECK(Hash<double>{}(1.0) != Hash<double>{}(2.0));
    }

    void TestHashCustomStruct()
    {
        // Своя структура через HashCombineArgs, вместо макроса Jolt JPH_MAKE_HASHABLE
        struct PairHash
        {
            KernelTypes::UInt64 operator()(const Pair& Value) const { return KernelContainers::HashCombineArgs(Value.First, Value.Second); }
        };

        CHECK(PairHash{}(Pair{1, 2.0f}) == PairHash{}(Pair{1, 2.0f}));
        CHECK(PairHash{}(Pair{1, 2.0f}) != PairHash{}(Pair{2, 1.0f}));
    }

    void TestHashOfArrays()
    {
        FastArray<int> A = FastArray<int>::Of(1, 2, 3);
        FastArray<int> B = FastArray<int>::Of(1, 2, 3);
        FastArray<int> C = FastArray<int>::Of(1, 2, 4);
        FastArray<int> D = FastArray<int>::Of(1, 2);
        FastArray<int> Empty;

        CHECK(A.GetHash() == B.GetHash());
        CHECK(A.GetHash() != C.GetHash());
        CHECK(A.GetHash() != D.GetHash());
        CHECK(Hash<FastArray<int>>{}(A) == A.GetHash());   // через GetHash
        CHECK(Empty.GetHash() == FastArray<int>().GetHash());

        StaticArray<int, 8> SA = StaticArray<int, 8>::Of(1, 2, 3);
        StaticArray<int, 8> SB = StaticArray<int, 8>::Of(1, 2, 3);
        StaticArray<int, 8> SC = StaticArray<int, 8>::Of(3, 2, 1);
        CHECK(SA.GetHash() == SB.GetHash());
        CHECK(SA.GetHash() != SC.GetHash());

        // Один и тот же набор в FastArray и StaticArray даёт один хеш
        CHECK(SA.GetHash() == A.GetHash());
    }

    void TestStaticConstructAndSize()
    {
        StaticArray<int, 8> Array;
        CHECK(Array.Size() == 0 && Array.Empty());
        CHECK(Array.GetCapacity() == 8);
        CHECK((StaticArray<int, 8>::MaxCount == 8));

        auto Filled = StaticArray<int, 8>::Of(1, 2, 3);
        CHECK(Filled.Size() == 3);
        CHECK(Filled[0] == 1 && Filled[1] == 2 && Filled[2] == 3);
    }

    void TestStaticPushPopEmplace()
    {
        StaticArray<int, 4> Array;
        Array.PushBack(10);
        Array.EmplaceBack(20);
        Array.PushBack(30);
        CHECK(Array.Size() == 3);
        CHECK(Array.Front() == 10 && Array.Back() == 30 && Array.At(1) == 20);

        Array.PopBack();
        CHECK(Array.Size() == 2 && Array.Back() == 20);

        Array.Clear();
        CHECK(Array.Empty());
    }

    void TestStaticFullCapacity()
    {
        StaticArray<int, 3> Array;
        for (int Index = 0; Index < 3; ++Index)
        {
            Array.PushBack(Index);
        }
        CHECK(Array.Size() == Array.GetCapacity());

        int Sum = 0;
        for (int Value : Array)
        {
            Sum += Value;
        }
        CHECK(Sum == 3);
    }

    void TestStaticNonTrivLifetime()
    {
        NonTriv::Reset();
        {
            StaticArray<NonTriv, 8> Array;
            CHECK(NonTriv::Constructors == 0);   // элементы не создаются сами

            Array.Resize(5);
            CHECK(Array.Size() == 5);
            CHECK(NonTriv::Constructors == 5);

            Array.Resize(2);
            CHECK(NonTriv::Destructors == 3);

            Array.EmplaceBack(7);
            CHECK(NonTriv::Constructors == 6);
            CHECK(Array.Back().Value() == 7);
        }
        CHECK(NonTriv::Destructors == 3 + 3);   // деструктор убрал оставшиеся три
        CHECK(NonTriv::Constructors + NonTriv::CopyConstructors + NonTriv::MoveConstructors == NonTriv::Destructors);
    }

    void TestStaticCopy()
    {
        NonTriv::Reset();
        StaticArray<NonTriv, 8> Source;
        Source.EmplaceBack(1);
        Source.EmplaceBack(2);
        Source.EmplaceBack(3);

        NonTriv::Reset();
        StaticArray<NonTriv, 8> Copy(Source);
        CHECK(Copy.Size() == 3);
        CHECK(Copy[0].Value() == 1 && Copy[2].Value() == 3);
        CHECK(NonTriv::CopyConstructors == 3);
        CHECK(Copy == Source);

        // Присваивание очищает прежнее содержимое
        StaticArray<NonTriv, 8> Target;
        Target.EmplaceBack(9);
        NonTriv::Reset();
        Target = Source;
        CHECK(NonTriv::Destructors == 1);
        CHECK(NonTriv::CopyConstructors == 3);
        CHECK(Target == Source);

        // Присваивание самому себе
        StaticArray<NonTriv, 8>& Alias = Target;   // через ссылку, чтобы компилятор не ругался на явное самоприсваивание
        Target = Alias;
        CHECK(Target.Size() == 3 && Target[1].Value() == 2);
    }

    void TestStaticAssignDifferentCapacity()
    {
        StaticArray<int, 4> Small = StaticArray<int, 4>::Of(1, 2, 3);
        StaticArray<int, 16> Large;
        Large = Small;
        CHECK(Large.Size() == 3 && Large[2] == 3);

        StaticArray<int, 4> Back;
        Back = Large;
        CHECK(Back == Small);
    }

    void TestStaticErase()
    {
        auto Array = StaticArray<int, 8>::Of(1, 2, 3, 4, 5);
        Array.Erase(Array.begin() + 1);
        CHECK(Array == (StaticArray<int, 8>::Of(1, 3, 4, 5)));

        Array.Erase(Array.begin() + 1, Array.begin() + 3);
        CHECK(Array == (StaticArray<int, 8>::Of(1, 5)));

        Array.Erase(Array.begin() + 1);
        CHECK(Array == (StaticArray<int, 8>::Of(1)));

        Array.Erase(Array.begin());
        CHECK(Array.Empty());
    }

    void TestStaticEraseNonTriv()
    {
        // В Jolt здесь был memmove, а для нетривиальных типов нужен поэлементный перенос
        StaticArray<NonTriv, 8> Array;
        for (int Index = 1; Index <= 5; ++Index)
        {
            Array.EmplaceBack(Index);
        }

        NonTriv::Reset();
        Array.Erase(Array.begin() + 1);
        CHECK(Array.Size() == 4);
        CHECK(Array[0].Value() == 1 && Array[1].Value() == 3 && Array[3].Value() == 5);
        CHECK(NonTriv::MoveConstructors == 3);
        CHECK(NonTriv::CopyConstructors == 0);

        Array.Erase(Array.begin(), Array.begin() + 2);
        CHECK(Array.Size() == 2);
        CHECK(Array[0].Value() == 4 && Array[1].Value() == 5);
    }

    void TestStaticCompare()
    {
        auto A = StaticArray<int, 8>::Of(1, 2, 3);
        auto B = StaticArray<int, 8>::Of(1, 2, 3);
        auto C = StaticArray<int, 8>::Of(1, 2, 4);
        auto D = StaticArray<int, 8>::Of(1, 2);

        CHECK(A == B && !(A != B));
        CHECK(A != C && !(A == C));
        CHECK(A != D);
    }

    void TestStaticResizeTrivial()
    {
        StaticArray<int, 8> Array;
        Array.Resize(5);
        CHECK(Array.Size() == 5);
        for (int Index = 0; Index < 5; ++Index)
        {
            Array[Index] = Index;
        }
        Array.Resize(3);
        CHECK(Array.Size() == 3 && Array[2] == 2);
    }
}

int RunContainersUnitTests()
{
    namespace KM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelMemory;
    const auto BaseBlocks = KM::GetLiveBlockCount();

    TestHashBytesAndString();
    TestHash64();
    TestHashCombine();
    TestHashFloat();
    TestHashCustomStruct();
    TestHashOfArrays();
    TestStaticConstructAndSize();
    TestStaticPushPopEmplace();
    TestStaticFullCapacity();
    TestStaticNonTrivLifetime();
    TestStaticCopy();
    TestStaticAssignDifferentCapacity();
    TestStaticErase();
    TestStaticEraseNonTriv();
    TestStaticCompare();
    TestStaticResizeTrivial();

    CHECK(KM::GetLiveBlockCount() == BaseBlocks);

    std::printf("Containers (Hash, StaticArray): %d проверок, %d ошибок\n", Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
