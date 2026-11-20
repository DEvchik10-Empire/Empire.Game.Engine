// Тесты планировщика задач. Три случая перенесены из JobSystemTest Jolt (UnitTests/Core), остальные написаны для движка:
// однопоточный планировщик, пакетное снятие зависимостей, стресс с вложенными задачами, переключение числа потоков.
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Jobs.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemSingleThreaded.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Jobs/Jobs.JobSystemThreadPool.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Profiler/Profiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include <chrono>
#include <cstdio>
#include <thread>

namespace KernelSync = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelSync;
namespace KernelTypes = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
namespace KernelContainers = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
using namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelJobs;
namespace KernelProfiler = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelProfiler;
using DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers::StaticArray;

static KernelSync::Atomic<int> Failures{0};
static KernelSync::Atomic<int> Checks{0};
#define CHECK(c) do { Checks.FetchAdd(1); if (!(c)) { Failures.FetchAdd(1); std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

using KernelSync::Atomic;
using KernelTypes::UInt32;

static void TestRunJobs()
{
    const int MaxJobs = 127;
    const int MaxBarriers = 10;
    const int MaxThreads = 10;
    JobSystemThreadPool System(MaxJobs + 1 /* одна задача зарезервирована, см. документацию */, MaxBarriers, MaxThreads);

    Atomic<UInt32> Values[MaxJobs];
    for (int I = 0; I < MaxJobs; ++I)
        Values[I].Store(0);

    JobSystem::Barrier* Barrier = System.CreateBarrier();

    for (int I = 0; I < MaxJobs; ++I)
    {
        JobHandle Handle = System.CreateJob("JobTest", 0xffff0000, [&Values, I] { Values[I].FetchAdd(1); });
        Barrier->AddJob(Handle);
    }

    System.WaitForJobs(Barrier);
    System.DestroyBarrier(Barrier);

    for (int I = 0; I < MaxJobs; ++I)
        CHECK(Values[I].Load() == 1);
}

static void TestRunChain()
{
    const int MaxJobs = 127;
    const int MaxBarriers = 10;
    JobSystemThreadPool System(MaxJobs + 1, MaxBarriers);

    JobSystem::Barrier* Barrier = System.CreateBarrier();

    Atomic<UInt32> Counter{1};
    Atomic<UInt32> Values[MaxJobs];
    for (int I = 0; I < MaxJobs; ++I)
        Values[I].Store(0);

    JobHandle Handles[MaxJobs];
    for (int I = 0; I < MaxJobs; ++I)
    {
        Handles[I] = System.CreateJob("JobTestChain", 0xffff0000, [&Values, &Counter, &Handles, I]
        {
            Values[I].Store(Counter.FetchAdd(1));

            // Запускаем предыдущую задачу
            if (I > 0)
                Handles[I - 1].RemoveDependency();
        }, 1);

        Barrier->AddJob(Handles[I]);
    }

    // Запускаем последнюю
    Handles[MaxJobs - 1].RemoveDependency();

    System.WaitForJobs(Barrier);
    System.DestroyBarrier(Barrier);

    // Задачи выполнились в обратном порядке
    for (int I = MaxJobs - 1; I >= 0; --I)
        CHECK(Values[I].Load() == static_cast<UInt32>(MaxJobs - I));
}

// Задачи, добавленные в барьер изнутри другой задачи, не должны освобождать ожидание раньше времени (issue #2198 в Jolt)
static void TestBarrierDoesNotReleaseEarly()
{
    constexpr int MaxJobs = 127;
    constexpr int MaxBarriers = 10;
    constexpr int ThreadCount = 4;
    constexpr int Iterations = 50;
    constexpr UInt32 AddedJobs = 4;

    JobSystemThreadPool System(MaxJobs + 1, MaxBarriers, ThreadCount);
    JobSystem::Barrier* Barrier = System.CreateBarrier();

    for (int Iteration = 0; Iteration < Iterations; ++Iteration)
    {
        Atomic<bool> AdderDone{false};
        Atomic<UInt32> AddedDone{0};

        // Задача, добавляющая задачи в барьер. У неё есть зависимость, поэтому в момент начала ожидания она не запускаема
        // и выполнится на рабочем потоке, а не на ждущем.
        JobHandle Adder = System.CreateJob("Adder", 0xffff0000, [&System, Barrier, &AdderDone, &AddedDone]
        {
            JobHandle Handles[AddedJobs];
            for (JobHandle& H : Handles)
                H = System.CreateJob("Added", 0xff00ff00, [&AddedDone] { AddedDone.FetchAdd(1, KernelSync::MemoryOrder::Relaxed); });
            Barrier->AddJobs(Handles, AddedJobs);

            // Последнее, что делает эта задача
            AdderDone.Store(true, KernelSync::MemoryOrder::Relaxed);
        }, 1);
        Barrier->AddJob(Adder);

        // Запускаем после того, как главный поток успел уснуть в WaitForJobs
        std::thread Starter([&Adder]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            Adder.RemoveDependency();
        });

        System.WaitForJobs(Barrier);
        bool AdderFinished = AdderDone.Load(KernelSync::MemoryOrder::Relaxed);
        UInt32 AddedFinished = AddedDone.Load(KernelSync::MemoryOrder::Relaxed);
        Starter.join();

        CHECK(AdderFinished);
        CHECK(AddedFinished == AddedJobs);
    }

    System.DestroyBarrier(Barrier);
}

static void TestJobProfilerIntegration()
{
    KernelProfiler::Profiler::NextFrame();
    KernelProfiler::Profiler::SetSamplingEnabled(true);
    {
        JobSystemSingleThreaded System(8);
        JobHandle Handle = System.CreateJob("ProfiledJob", 0x12345678, [] {});
        CHECK(Handle.IsDone());
    }

    const auto Samples = KernelProfiler::Profiler::GetSamples();
    CHECK(Samples.GetCount() == 1);
    if (Samples.GetCount() == 1)
    {
        CHECK(KernelContainers::StringView(Samples[0].Name) == KernelContainers::StringView("ProfiledJob"));
        CHECK(Samples[0].Color == 0x12345678);
        CHECK(Samples[0].EndTick >= Samples[0].StartTick);
    }
    KernelProfiler::Profiler::NextFrame();
    KernelProfiler::Profiler::SetSamplingEnabled(false);
}

static void TestSingleThreaded()
{
    JobSystemSingleThreaded System(16);
    CHECK(System.GetMaxConcurrency() == 1);

    int Value = 0;
    JobHandle A = System.CreateJob("A", 0, [&Value] { Value += 1; });
    CHECK(A.IsDone() && Value == 1);

    // Задача с зависимостью ждёт, пока её не снимут
    JobHandle B = System.CreateJob("B", 0, [&Value] { Value *= 10; }, 2);
    CHECK(!B.IsDone() && Value == 1);
    B.RemoveDependency();
    CHECK(!B.IsDone());
    B.RemoveDependency();
    CHECK(B.IsDone() && Value == 10);

    // AddDependency до запуска
    JobHandle C = System.CreateJob("C", 0, [&Value] { Value += 5; }, 1);
    C.AddDependency();
    C.RemoveDependency(2);
    CHECK(C.IsDone() && Value == 15);

    JobHandle Many[3];
    System.CreateJobs("Many", 0, [&Value] { Value += 1; }, Many, 3);
    CHECK(Value == 18 && Many[0].IsDone() && Many[2].IsDone());

    JobSystem::Barrier* Barrier = System.CreateBarrier();
    Barrier->AddJob(A);
    System.WaitForJobs(Barrier);
    System.DestroyBarrier(Barrier);
}

static void TestBatchRemoveDependencies()
{
    // Больше 64 задач, чтобы сработало разбиение на пачки
    constexpr int Count = 150;
    JobSystemThreadPool System(256, 2, 3);
    JobSystem::Barrier* Barrier = System.CreateBarrier();

    Atomic<UInt32> Ran{0};
    static JobHandle Handles[Count];
    for (int I = 0; I < Count; ++I)
    {
        Handles[I] = System.CreateJob("Batch", 0, [&Ran] { Ran.FetchAdd(1); }, 1);
        Barrier->AddJob(Handles[I]);
    }
    CHECK(Ran.Load() == 0);

    JobHandle::RemoveDependencies(Handles, Count);
    System.WaitForJobs(Barrier);
    System.DestroyBarrier(Barrier);
    CHECK(Ran.Load() == Count);

    StaticArray<JobHandle, 4> Small;
    Atomic<UInt32> Ran2{0};
    JobSystemSingleThreaded Single(8);
    for (int I = 0; I < 4; ++I)
        Small.PushBack(Single.CreateJob("S", 0, [&Ran2] { Ran2.FetchAdd(1); }, 1));
    JobHandle::RemoveDependencies(Small);
    CHECK(Ran2.Load() == 4);
    Small.Clear();   // хендлы должны быть отпущены до уничтожения планировщика

    for (int I = 0; I < Count; ++I)
        Handles[I] = JobHandle();
}

static void TestStress()
{
    // Много раундов: пачки задач, пересоздание барьера, повторное использование пула
    // Пул с запасом на все 8000 задач: на одном ядре воркеры могут не успеть отпустить задачи между раундами
    JobSystemThreadPool System(8192, 4, 3);
    CHECK(System.GetMaxConcurrency() == 4);

    Atomic<UInt32> Sum{0};
    for (int Round = 0; Round < 200; ++Round)
    {
        JobSystem::Barrier* Barrier = System.CreateBarrier();
        JobHandle Jobs[40];
        System.CreateJobs("Stress", 0, [&Sum] { Sum.FetchAdd(1, KernelSync::MemoryOrder::Relaxed); }, Jobs, 40);
        Barrier->AddJobs(Jobs, 40);
        System.WaitForJobs(Barrier);
        System.DestroyBarrier(Barrier);
    }
    CHECK(Sum.Load() == 200 * 40);

    // Смена числа потоков и функции инициализации/завершения
    Atomic<UInt32> Started{0}, Exited{0};
    {
        JobSystemThreadPool Pool;
        Pool.SetThreadInitFunction([&Started](int) { Started.FetchAdd(1); });
        Pool.SetThreadExitFunction([&Exited](int) { Exited.FetchAdd(1); });
        Pool.Init(64, 2, 2);
        JobSystem::Barrier* Barrier = Pool.CreateBarrier();
        JobHandle H = Pool.CreateJob("X", 0, [] {});
        Barrier->AddJob(H);
        Pool.WaitForJobs(Barrier);
        Pool.DestroyBarrier(Barrier);
        CHECK(Pool.GetMaxConcurrency() == 3);

        Pool.SetThreadCount(0);
        CHECK(Pool.GetMaxConcurrency() == 1);

        // Без рабочих потоков барьер выполняет задачи сам
        Barrier = Pool.CreateBarrier();
        Atomic<UInt32> Own{0};
        JobHandle Y = Pool.CreateJob("Y", 0, [&Own] { Own.FetchAdd(1); });
        Barrier->AddJob(Y);
        Pool.WaitForJobs(Barrier);
        Pool.DestroyBarrier(Barrier);
        CHECK(Own.Load() == 1);
    }
    CHECK(Started.Load() == 2 && Exited.Load() == 2);
}

int RunJobsUnitTests()
{
    Failures.Store(0);
    Checks.Store(0);
    TestRunJobs();
    TestRunChain();
    TestBarrierDoesNotReleaseEarly();
    TestJobProfilerIntegration();
    TestSingleThreaded();
    TestBatchRemoveDependencies();
    TestStress();
    std::printf("Jobs: %d checks, %d failures\n", Checks.Load(), Failures.Load());
    return Failures.Load() == 0 ? 0 : 1;
}
