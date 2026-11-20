// Тесты FixedSizeFreeList и LockFreeHashMap. В Jolt для них нет модульных тестов (их там проверяют только через физику),
// поэтому тесты написаны для движка: однопоточная логика плюс многопоточные сценарии (запускать также под ThreadSanitizer).
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Concurrent.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FixedSizeFreeList.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.LockFreeHashMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include <cstdio>
#include <thread>
#include <vector>

namespace KernelTypes = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
namespace KernelSync = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelSync;
namespace KernelCompiler = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler;
namespace KernelContainers = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
using namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;

static_assert(requires(KernelContainers::LockFreeHashMap<int, int>& Map) {
    Map.GetNumBuckets();
    Map.GetMaxBuckets();
    Map.SetNumBuckets(4);
    Map.GetNumKeyValues();
    Map.GetStatistics();
});

static KernelSync::Atomic<int> Failures{0};
static KernelSync::Atomic<int> Checks{0};
#define CHECK(c) do { Checks.FetchAdd(1); if (!(c)) { Failures.FetchAdd(1); std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

namespace
{
    struct Tracked
    {
        static inline KernelSync::Atomic<int> Alive{0};
        int Value;
        explicit Tracked(int V) : Value(V) { Alive.FetchAdd(1); }
        Tracked(const Tracked&) = delete;
        ~Tracked() { Alive.FetchSub(1); }
    };

    constexpr int ThreadCount = 4;

    template <typename F>
    void RunThreads(F Body)
    {
        std::vector<std::thread> Threads;
        for (int T = 0; T < ThreadCount; ++T)
            Threads.emplace_back([&Body, T] { Body(T); });
        for (auto& Thread : Threads)
            Thread.join();
    }
}

static void TestFreeListBasic()
{
    FixedSizeFreeList<Tracked> List;
    List.Init(100, 16);

    KernelTypes::UInt32 A = List.ConstructObject(5);
    KernelTypes::UInt32 B = List.ConstructObject(6);
    CHECK(A != B);
    CHECK(List.Get(A).Value == 5);
    CHECK(List.Get(B).Value == 6);
    CHECK(List.GetObjectIndex(&List.Get(B)) == B);
    CHECK(Tracked::Alive.Load() == 2);

    List.DestructObject(A);
    CHECK(Tracked::Alive.Load() == 1);

    // Освобождённый индекс используется повторно
    KernelTypes::UInt32 C = List.ConstructObject(7);
    CHECK(C == A);
    CHECK(List.Get(C).Value == 7);

    // Пакет
    FixedSizeFreeList<Tracked>::Batch Batch;
    List.AddObjectToBatch(Batch, B);
    List.AddObjectToBatch(Batch, C);
    CHECK(Batch.ObjectCount == 2);
    List.DestructObjectBatch(Batch);
    CHECK(Tracked::Alive.Load() == 0);

    // Исчерпание пула: 100 объектов округляются до 7 страниц по 16
    std::vector<KernelTypes::UInt32> All;
    for (;;)
    {
        KernelTypes::UInt32 I = List.ConstructObject(1);
        if (I == FixedSizeFreeList<Tracked>::InvalidObjectIndex)
            break;
        All.push_back(I);
    }
    CHECK(All.size() == 112);
    for (auto I : All)
        List.DestructObject(&List.Get(I));
    CHECK(Tracked::Alive.Load() == 0);
}

static void TestFreeListThreads()
{
    FixedSizeFreeList<Tracked> List;
    List.Init(4096, 64);

    RunThreads([&](int Thread)
    {
        std::vector<KernelTypes::UInt32> Mine;
        for (int Round = 0; Round < 200; ++Round)
        {
            for (int I = 0; I < 20; ++I)
            {
                KernelTypes::UInt32 Index = List.ConstructObject(Thread * 1000000 + Round * 100 + I);
                CHECK(Index != FixedSizeFreeList<Tracked>::InvalidObjectIndex);
                Mine.push_back(Index);
            }
            // Объекты не должны портиться чужими потоками
            for (size_t I = 0; I < Mine.size(); ++I)
                CHECK(List.Get(Mine[I]).Value / 1000000 == Thread);
            if (Round % 2 == 0)
            {
                FixedSizeFreeList<Tracked>::Batch Batch;
                for (auto Index : Mine)
                    List.AddObjectToBatch(Batch, Index);
                List.DestructObjectBatch(Batch);
            }
            else
            {
                for (auto Index : Mine)
                    List.DestructObject(Index);
            }
            Mine.clear();
        }
    });
    CHECK(Tracked::Alive.Load() == 0);
}

static void TestLockFreeMapBasic()
{
    LockFreeAllocator Allocator;
    Allocator.Init(1 << 16);
    LockFreeHashMap<int, int> Map(Allocator);
    CHECK(Map.begin() == Map.end());
    FastArray<const LockFreeHashMap<int, int>::KeyValue*> UninitializedValues;
    Map.GetAllKeyValues(UninitializedValues);
    CHECK(UninitializedValues.Empty());
    CHECK(Map.GetStatistics().BucketCount == 0);
    Map.Init(64);
    LockFreeAllocatorContext Context(Allocator, 256);

    Hash<int> Hasher;
    for (int I = 0; I < 100; ++I)
    {
        auto* Pair = Map.Create(Context, I, Hasher(I), 0, I * 2);
        CHECK(Pair != nullptr);
    }
    for (int I = 0; I < 100; ++I)
    {
        auto* Pair = Map.Find(I, Hasher(I));
        CHECK(Pair != nullptr && Pair->GetValue() == I * 2 && Pair->GetKey() == I);
        CHECK(Map.FromHandle(Map.ToHandle(Pair)) == Pair);
    }
    CHECK(Map.Find(1000, Hasher(1000)) == nullptr);

    FastArray<const LockFreeHashMap<int, int>::KeyValue*> All;
    Map.GetAllKeyValues(All);
    CHECK(All.Size() == 100);
    const auto Statistics = Map.GetStatistics();
    CHECK(Statistics.BucketCount == 64 && Statistics.ObjectCount == 100 && Statistics.MaxObjectsPerBucket > 0);
    KernelTypes::UInt32 HistogramBuckets = 0;
    for (KernelTypes::UInt32 BucketCount : Statistics.Histogram)
        HistogramBuckets += BucketCount;
    CHECK(HistogramBuckets == 64);

    int Count = 0;
    long Sum = 0;
    for (auto& Pair : Map)
    {
        ++Count;
        Sum += Pair.GetKey();
    }
    CHECK(Count == 100);
    CHECK(Sum == 4950);

    // Очистка и смена числа корзин
    Map.Clear();
    Allocator.Clear();
    Map.SetBucketCount(16);
    CHECK(Map.GetBucketCount() == 16);
    CHECK(Map.GetMaxBucketCount() == 64);
    const auto EmptyStatistics = Map.GetStatistics();
    CHECK(EmptyStatistics.BucketCount == 16 && EmptyStatistics.ObjectCount == 0 && EmptyStatistics.Histogram[0] == 16);
    CHECK(Map.Find(5, Hasher(5)) == nullptr);
    CHECK(Map.begin() == Map.end());
}

static void TestLockFreeMapFull()
{
    LockFreeAllocator Allocator;
    Allocator.Init(1024);
    LockFreeHashMap<int, int> Map(Allocator);
    Map.Init(16);
    LockFreeAllocatorContext Context(Allocator, 128);
    Hash<int> Hasher;

    int Inserted = 0;
    for (int I = 0; I < 1000; ++I)
        if (Map.Create(Context, I, Hasher(I), 0, I) != nullptr)
            ++Inserted;
    // Пара занимает 12 байт (ключ, смещение, значение)
    CHECK(Inserted > 50 && Inserted <= 1024 / 12);
    for (int I = 0; I < Inserted; ++I)
        CHECK(Map.Find(I, Hasher(I)) != nullptr);
}

static void TestLockFreeMapThreads()
{
    constexpr int PerThread = 5000;
    LockFreeAllocator Allocator;
    Allocator.Init(ThreadCount * PerThread * 16 + 65536);
    LockFreeHashMap<int, int> Map(Allocator);
    Map.Init(1024);
    Hash<int> Hasher;

    RunThreads([&](int Thread)
    {
        LockFreeAllocatorContext Context(Allocator, 1024);
        for (int I = 0; I < PerThread; ++I)
        {
            int Key = Thread * PerThread + I;
            CHECK(Map.Create(Context, Key, Hasher(Key), 0, Key + 1) != nullptr);
            // Свои ключи видны сразу
            auto* Pair = Map.Find(Key, Hasher(Key));
            CHECK(Pair != nullptr && Pair->GetValue() == Key + 1);
        }
    });

    for (int Key = 0; Key < ThreadCount * PerThread; ++Key)
    {
        auto* Pair = Map.Find(Key, Hasher(Key));
        CHECK(Pair != nullptr && Pair->GetValue() == Key + 1);
    }
    CHECK(Map.GetKeyValueCount() == (KernelCompiler::IsDebugBuild ? ThreadCount * PerThread : 0));
}

int RunConcurrentUnitTests()
{
    Failures.Store(0);
    Checks.Store(0);
    TestFreeListBasic();
    TestFreeListThreads();
    TestLockFreeMapBasic();
    TestLockFreeMapFull();
    TestLockFreeMapThreads();
    std::printf("Concurrent: %d checks, %d failures\n", Checks.Load(), Failures.Load());
    return Failures.Load() == 0 ? 0 : 1;
}
