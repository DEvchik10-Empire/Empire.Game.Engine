// Тесты Thread и Semaphore. В Jolt модульных тестов для Semaphore нет (его проверяют через JobSystemTest), тесты написаны для движка.
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Threading.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Threading/Threading.hpp"
#include <cstdio>

namespace KernelThreading = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelThreading;
namespace KernelSync = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelSync;

static KernelSync::Atomic<int> Failures{0};
static KernelSync::Atomic<int> Checks{0};
#define CHECK(c) do { Checks.FetchAdd(1); if (!(c)) { Failures.FetchAdd(1); std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

namespace
{
    struct Shared
    {
        KernelSync::Atomic<int> Counter{0};
        KernelThreading::Semaphore Start;
        KernelThreading::Semaphore Done;
    };

    void CountEntry(void* Context)
    {
        Shared& S = *static_cast<Shared*>(Context);
        S.Start.Acquire();
        for (int I = 0; I < 1000; ++I)
            S.Counter.FetchAdd(1);
        S.Done.Release();
    }
}

static void TestThreadBasic()
{
    CHECK(KernelThreading::GetHardwareThreadCount() >= 1);

    Shared S;
    constexpr int N = 4;
    KernelThreading::Thread Threads[N];
    for (auto& T : Threads)
    {
        CHECK(!T.IsJoinable());
        CHECK(T.Start(CountEntry, &S, "TestWorker"));
        CHECK(T.IsJoinable());
    }

    // Потоки ждут на семафоре, пока не отпустим все сразу
    CHECK(S.Counter.Load() == 0);
    S.Start.Release(N);
    S.Done.Acquire(N);
    CHECK(S.Counter.Load() == N * 1000);

    for (auto& T : Threads)
    {
        T.Join();
        CHECK(!T.IsJoinable());
    }

    // Перемещение
    KernelThreading::Thread A;
    CHECK(A.Start([](void* C) { static_cast<KernelSync::Atomic<int>*>(C)->Store(7); }, &S.Counter));
    KernelThreading::Thread B(static_cast<KernelThreading::Thread&&>(A));
    CHECK(!A.IsJoinable() && B.IsJoinable());
    B.Join();
    CHECK(S.Counter.Load() == 7);
}

static void TestSemaphoreSingleThread()
{
    KernelThreading::Semaphore Sem;
    CHECK(Sem.GetValue() == 0);
    Sem.Release(3);
    CHECK(Sem.GetValue() == 3);
    Sem.Acquire();
    Sem.Acquire(2);
    CHECK(Sem.GetValue() == 0);
}

namespace
{
    struct PingPong
    {
        KernelThreading::Semaphore ToWorker;
        KernelThreading::Semaphore ToMain;
        int Rounds = 0;
        KernelSync::Atomic<int> Handled{0};
    };

    void PingPongEntry(void* Context)
    {
        PingPong& P = *static_cast<PingPong*>(Context);
        for (int I = 0; I < P.Rounds; ++I)
        {
            P.ToWorker.Acquire();
            P.Handled.FetchAdd(1);
            P.ToMain.Release();
        }
    }
}

static void TestSemaphorePingPong()
{
    PingPong P;
    P.Rounds = 20000;
    KernelThreading::Thread Worker;
    CHECK(Worker.Start(PingPongEntry, &P, "PingPong"));
    for (int I = 0; I < P.Rounds; ++I)
    {
        P.ToWorker.Release();
        P.ToMain.Acquire();
    }
    Worker.Join();
    CHECK(P.Handled.Load() == P.Rounds);
    CHECK(P.ToWorker.GetValue() == 0 && P.ToMain.GetValue() == 0);
}

static void TestAtomicMinMax()
{
    KernelSync::Atomic<int> Value{10};
    CHECK(KernelSync::AtomicMin(Value, 5) && Value.Load() == 5);
    CHECK(!KernelSync::AtomicMin(Value, 7) && Value.Load() == 5);
    CHECK(!KernelSync::AtomicMin(Value, 5) && Value.Load() == 5);
    CHECK(KernelSync::AtomicMax(Value, 20) && Value.Load() == 20);
    CHECK(!KernelSync::AtomicMax(Value, 19) && Value.Load() == 20);
    CHECK(!KernelSync::AtomicMax(Value, 20) && Value.Load() == 20);

    // Беззнаковые и с явным порядком памяти
    KernelSync::Atomic<unsigned> Unsigned{100};
    CHECK(KernelSync::AtomicMin(Unsigned, 3u, KernelSync::MemoryOrder::Relaxed) && Unsigned.Load() == 3u);
    CHECK(KernelSync::AtomicMax(Unsigned, 4000000000u, KernelSync::MemoryOrder::AcquireRelease) && Unsigned.Load() == 4000000000u);
}

static void TestIsLocked()
{
    KernelSync::Mutex Plain;
    CHECK(!Plain.IsLocked());
    Plain.Lock();
    CHECK(Plain.IsLocked());
    Plain.Unlock();
    CHECK(!Plain.IsLocked());

    KernelSync::SharedMutex Shared;
    CHECK(!Shared.IsLocked());
    Shared.LockShared();
    CHECK(Shared.IsLocked());
    Shared.UnlockShared();
    CHECK(!Shared.IsLocked());
    Shared.Lock();
    CHECK(Shared.IsLocked());
    Shared.Unlock();
    CHECK(!Shared.IsLocked());
}

int RunThreadingUnitTests()
{
    Failures.Store(0);
    Checks.Store(0);
    TestThreadBasic();
    TestSemaphoreSingleThread();
    TestSemaphorePingPong();
    TestAtomicMinMax();
    TestIsLocked();
    std::printf("Threading: %d checks, %d failures\n", Checks.Load(), Failures.Load());
    return Failures.Load() == 0 ? 0 : 1;
}
