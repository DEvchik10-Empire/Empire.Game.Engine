#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
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

                    struct Float3
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;
                        KernelTypes::Float Z;

                        constexpr Float3() noexcept = default;
                        constexpr Float3(KernelTypes::Float XValue, KernelTypes::Float YValue, KernelTypes::Float ZValue) noexcept
                            : X(XValue), Y(YValue), Z(ZValue)
                        {
                        }

                        [[nodiscard]] constexpr KernelTypes::Float operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            return Index == 0 ? X : (Index == 1 ? Y : Z);
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Float3& Other) const noexcept
                        {
                            return X == Other.X && Y == Other.Y && Z == Other.Z;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Float3& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        [[nodiscard]] KernelTypes::UInt64 GetHash() const noexcept
                        {
                            return KernelContainers::HashCombineArgs(X, Y, Z);
                        }
                    };

                    using VertexList = KernelContainers::Array<Float3>;

                    static_assert(sizeof(Float3) == sizeof(KernelTypes::Float) * 3);
                    static_assert(KernelCompiler::IsTriviallyConstructible<Float3>);
                    static_assert(KernelCompiler::IsTriviallyCopyable<Float3>);
                }
            }
        }
    }
}
