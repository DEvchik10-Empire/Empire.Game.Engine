#pragma once

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

                    struct alignas(16) Float4
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;
                        KernelTypes::Float Z;
                        KernelTypes::Float W;

                        constexpr Float4() noexcept = default;
                        constexpr Float4(KernelTypes::Float XValue, KernelTypes::Float YValue, KernelTypes::Float ZValue, KernelTypes::Float WValue) noexcept
                            : X(XValue), Y(YValue), Z(ZValue), W(WValue)
                        {
                        }

                        [[nodiscard]] constexpr KernelTypes::Float operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            return Index == 0 ? X : (Index == 1 ? Y : (Index == 2 ? Z : W));
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Float4& Other) const noexcept
                        {
                            return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Float4& Other) const noexcept
                        {
                            return !(*this == Other);
                        }
                    };

                    static_assert(sizeof(Float4) == 16);
                    static_assert(alignof(Float4) == 16);
                    static_assert(KernelCompiler::IsTriviallyConstructible<Float4>);
                    static_assert(KernelCompiler::IsTriviallyCopyable<Float4>);
                }
            }
        }
    }
}
