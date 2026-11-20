#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Algorithms.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Concurrent.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Containers.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Compiler.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.FastArray.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.HashTables.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Kernel.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Jobs.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Reference.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.TempAllocator.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Threading.hpp"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    const int KernelResult = RunKernelUnitTests();
    const int CompilerResult = RunCompilerUnitTests();
    const int AlgorithmsResult = RunAlgorithmsUnitTests();
    const int FastArrayResult = RunFastArrayUnitTests();
    const int ContainersResult = RunContainersUnitTests();
    const int HashTablesResult = RunHashTablesUnitTests();
    const int ConcurrentResult = RunConcurrentUnitTests();
    const int TempAllocatorResult = RunTempAllocatorUnitTests();
    const int ThreadingResult = RunThreadingUnitTests();
    const int ReferenceResult = RunReferenceUnitTests();
    const int JobsResult = RunJobsUnitTests();

    if (KernelResult != 0)
    {
        return KernelResult;
    }

    if (CompilerResult != 0)
    {
        return CompilerResult;
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

    if (ConcurrentResult != 0)
    {
        return ConcurrentResult;
    }

    if (TempAllocatorResult != 0)
    {
        return TempAllocatorResult;
    }

    if (ThreadingResult != 0)
    {
        return ThreadingResult;
    }

    return ReferenceResult != 0 ? ReferenceResult : JobsResult;
}
