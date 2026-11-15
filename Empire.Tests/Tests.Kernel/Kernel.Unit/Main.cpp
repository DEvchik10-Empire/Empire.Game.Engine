#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Algorithms.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Concurrent.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Containers.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.FastArray.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.HashTables.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Kernel.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.TempAllocator.hpp"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    const int KernelResult = RunKernelUnitTests();
    const int AlgorithmsResult = RunAlgorithmsUnitTests();
    const int FastArrayResult = RunFastArrayUnitTests();
    const int ContainersResult = RunContainersUnitTests();
    const int HashTablesResult = RunHashTablesUnitTests();
    const int ConcurrentResult = RunConcurrentUnitTests();
    const int TempAllocatorResult = RunTempAllocatorUnitTests();

    if (KernelResult != 0)
    {
        return KernelResult;
    }

    if (AlgorithmsResult != 0)
    {
        return AlgorithmsResult;
    }

    if (FastArrayResult != 0)
    {
        return FastArrayResult;
    }

    if (ContainersResult != 0)
    {
        return ContainersResult;
    }

    if (HashTablesResult != 0)
    {
        return HashTablesResult;
    }

    return ConcurrentResult != 0 ? ConcurrentResult : TempAllocatorResult;
}
