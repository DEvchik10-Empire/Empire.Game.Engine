#include "Empire.Kernel/Kernel.Assert/Assert.hpp"

#include <cstdio>
#include <cstdlib>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Assert
            {
                void Check(Bool Condition, const Char* Message, const std::source_location& Location)
                {
                    if (!Condition)
                        Fail(Message, Location);
                }

                [[noreturn]] void Fail(const Char* Message, const std::source_location& Location)
                {
                    std::fprintf(
                        stderr,
                        "Empire assertion failed: %s\nFile: %s\nFunction: %s\nLine: %u\n",
                        Message != nullptr ? Message : "No message",
                        Location.file_name(),
                        Location.function_name(),
                        Location.line()
                    );

                    std::fflush(stderr);
                    std::abort();
                }
            }
        }
    }
}
