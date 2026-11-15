#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Algorithms.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Containers.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.FastArray.hpp"
#include "Empire.Tests/Tests.Kernel/Kernel.Unit/Unit.Kernel.hpp"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    const int KernelResult = RunKernelUnitTests();
    const int AlgorithmsResult = RunAlgorithmsUnitTests();
    const int FastArrayResult = RunFastArrayUnitTests();
    const int ContainersResult = RunContainersUnitTests();

    if (KernelResult != 0)
    {
        return KernelResult;
    }

    if (AlgorithmsResult != 0)
    {
        return AlgorithmsResult;
    }

    return FastArrayResult != 0 ? FastArrayResult : ContainersResult;
}
