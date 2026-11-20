#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathVector
                {
                    using namespace EngineKernel;

                    enum class SwizzleComponent : KernelTypes::UInt32
                    {
                        X = 0,
                        Y = 1,
                        Z = 2,
                        W = 3,
                        Unused = Z
                    };
                }
            }
        }
    }
}
