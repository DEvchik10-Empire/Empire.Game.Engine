// Тесты Ref, RefConst и RefTarget. В Jolt отдельного теста для Reference нет, тест написан для движка
// (включая многопоточное освобождение одного объекта, его стоит гонять под ThreadSanitizer).
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Reference.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastHashSet.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.Reference.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include <cstdio>
#include <thread>
#include <vector>

namespace KernelMemory = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelMemory;
namespace KernelSync = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelSync;
using namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
using KernelMemory::Ref;
using KernelMemory::RefConst;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

namespace
{
    int Alive = 0;
    KernelSync::Atomic<int> AliveShared{0};

    class Thing : public KernelMemory::RefTarget<Thing>
    {
        public:
            Thing() { ++Alive; AliveShared.FetchAdd(1); }
            Thing(const Thing& Other) : KernelMemory::RefTarget<Thing>(Other) { ++Alive; AliveShared.FetchAdd(1); }
            ~Thing() { --Alive; AliveShared.FetchSub(1); }
            int Value = 0;
    };
}

static void TestBasic()
{
    Ref<Thing> A = new Thing;
    CHECK(Alive == 1 && A->GetRefCount() == 1);

    Ref<Thing> B = A;
    CHECK(A->GetRefCount() == 2 && A == B);

    Ref<Thing> C = static_cast<Ref<Thing>&&>(B);
    CHECK(B == nullptr && C == A && A->GetRefCount() == 2);

    // Копирование объекта не копирует счётчик
    Thing* Copy = new Thing(*A);
    CHECK(Copy->GetRefCount() == 0);
    Ref<Thing> D = Copy;
    CHECK(D->GetRefCount() == 1);
    D = A;
    CHECK(Alive == 1 && A->GetRefCount() == 3);   // копия удалилась

    RefConst<Thing> K = A;
    CHECK(K.GetPtr() == A.GetPtr() && A->GetRefCount() == 4);
    K = nullptr;
    CHECK(A->GetRefCount() == 3);

    A = nullptr; C = nullptr;
    CHECK(Alive == 1);
    D = nullptr;
    CHECK(Alive == 0);

    // Самоприсваивание
    Ref<Thing> E = new Thing;
    Ref<Thing>& Alias = E;
    E = Alias;
    CHECK(E->GetRefCount() == 1);
    E = nullptr;
    CHECK(Alive == 0);
}

// Отдельная функция, чтобы компилятор не видел, что объект на стеке (иначе GCC ложно предупреждает об удалении стекового объекта,
// хотя Release встроенный объект никогда не удаляет)
[[gnu::noinline]] static int HoldReference(Thing* Pointer)
{
    Ref<Thing> R = Pointer;
    return static_cast<int>(Pointer->GetRefCount());
}

static void TestEmbedded()
{
    {
        Thing OnStack;
        OnStack.SetEmbedded();
        {
            CHECK(HoldReference(&OnStack) > 1);
        }
        // Release не удаляет встроенный объект
        CHECK(Alive == 1);
    }
    CHECK(Alive == 0);
}

static void TestHashSet()
{
    FastHashSet<Ref<Thing>> Set;
    Ref<Thing> A = new Thing, B = new Thing;
    Set.Insert(A);
    Set.Insert(B);
    Set.Insert(A);
    CHECK(Set.Size() == 2);
    CHECK(Set.Find(A) != Set.cend());
    CHECK(A->GetRefCount() == 2);
    Set.Clear();
    CHECK(A->GetRefCount() == 1);
}

static void TestThreads()
{
    Ref<Thing> Shared = new Thing;
    std::vector<std::thread> Threads;
    for (int T = 0; T < 4; ++T)
    {
        Threads.emplace_back([Mine = Shared]() mutable
        {
            for (int I = 0; I < 20000; ++I)
            {
                Ref<Thing> Local = Mine;
                Local->AddRef();
                Local->Release();
            }
        });
    }
    Shared = nullptr;   // потоки держат объект сами
    for (auto& T : Threads)
        T.join();
    CHECK(AliveShared.Load() == 0);
}

int RunReferenceUnitTests()
{
    Failures = 0;
    Checks = 0;
    TestBasic();
    TestEmbedded();
    TestHashSet();
    TestThreads();
    std::printf("Reference: %d checks, %d failures\n", Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
