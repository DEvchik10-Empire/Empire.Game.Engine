// Тесты TempAllocator. В Jolt модульного теста для него нет, тест написан для движка.
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.TempAllocator.hpp"

#include "Empire.Kernel/Kernel.Memory/Memory.TempAllocator.hpp"
#include <cstdint>
#include <cstdio>

namespace KernelMemory = DEvchik10Empire::EmpireGameEngine::EmpireKernel::KernelMemory;

static int Failures = 0, Checks = 0;
#define CHECK(c) do { ++Checks; if (!(c)) { ++Failures; std::printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); } } while (0)

static void TestImpl()
{
    KernelMemory::TempAllocatorImpl Allocator(1024);
    CHECK(Allocator.IsEmpty());
    CHECK(Allocator.GetSize() == 1024);
    CHECK(Allocator.Allocate(0) == nullptr);

    void* A = Allocator.Allocate(10);
    void* B = Allocator.Allocate(100);
    CHECK(reinterpret_cast<std::uintptr_t>(A) % 16 == 0);
    CHECK(reinterpret_cast<std::uintptr_t>(B) % 16 == 0);
    CHECK(static_cast<char*>(B) - static_cast<char*>(A) == 16);
    CHECK(Allocator.GetUsage() == 16 + 112);
    CHECK(Allocator.OwnsMemory(A) && !Allocator.OwnsMemory(&Allocator));
    CHECK(Allocator.CanAllocate(1024 - 128));
    CHECK(!Allocator.CanAllocate(1024 - 127));

    Allocator.Free(B, 100);
    Allocator.Free(A, 10);
    CHECK(Allocator.IsEmpty());

    // Нехватка памяти и неверный порядок освобождения завершают программу (KernelAssert::Fail), как abort() в Jolt, поэтому здесь не проверяются
    void* C = Allocator.Allocate(32);
    void* D = Allocator.Allocate(32);
    CHECK(Allocator.GetUsage() == 64);
    Allocator.Free(D, 32);
    Allocator.Free(C, 32);
    CHECK(Allocator.IsEmpty());
}

static void TestMallocAndFallback()
{
    KernelMemory::TempAllocatorMalloc Heap;
    void* P = Heap.Allocate(40);
    CHECK(P != nullptr && reinterpret_cast<std::uintptr_t>(P) % 16 == 0);
    Heap.Free(P, 40);
    CHECK(Heap.Allocate(0) == nullptr);

    KernelMemory::TempAllocatorImplWithMallocFallback Mixed(256);
    void* Small = Mixed.Allocate(64);
    void* Big = Mixed.Allocate(1000);   // не помещается, идёт в запасной аллокатор
    void* Small2 = Mixed.Allocate(64);
    CHECK(Small && Big && Small2);
    Mixed.Free(Small2, 64);
    Mixed.Free(Big, 1000);
    Mixed.Free(Small, 64);
    Mixed.Free(nullptr, 0);
}

int RunTempAllocatorUnitTests()
{
    Failures = 0;
    Checks = 0;
    TestImpl();
    TestMallocAndFallback();
    std::printf("TempAllocator: %d checks, %d failures\n", Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
