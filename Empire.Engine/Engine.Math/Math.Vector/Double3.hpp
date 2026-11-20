#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
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

                    struct Double3
                    {
                        KernelTypes::Double X;
                        KernelTypes::Double Y;
                        KernelTypes::Double Z;

                        constexpr Double3() noexcept = default;
                        constexpr Double3(KernelTypes::Double XValue, KernelTypes::Double YValue, KernelTypes::Double ZValue) noexcept
                            : X(XValue), Y(YValue), Z(ZValue)
                        {
                        }

                        [[nodiscard]] constexpr KernelTypes::Double operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            return Index == 0 ? X : (Index == 1 ? Y : Z);
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Double3& Other) const noexcept
                        {
                            return X == Other.X && Y == Other.Y && Z == Other.Z;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Double3& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        [[nodiscard]] KernelTypes::UInt64 GetHash() const noexcept
                        {
                            return KernelContainers::HashCombineArgs(X, Y, Z);
                        }
                    };

                    static_assert(sizeof(Double3) == sizeof(KernelTypes::Double) * 3);
                    static_assert(KernelCompiler::IsTriviallyConstructible<Double3>);
                    static_assert(KernelCompiler::IsTriviallyCopyable<Double3>);
                }
            }
        }
    }
}
