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

                    struct Float2
                    {
                        KernelTypes::Float X;
                        KernelTypes::Float Y;

                        constexpr Float2() noexcept = default;
                        constexpr Float2(KernelTypes::Float XValue, KernelTypes::Float YValue) noexcept : X(XValue), Y(YValue) {}

                        [[nodiscard]] constexpr KernelTypes::Float operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            return Index == 0 ? X : Y;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const Float2& Other) const noexcept
                        {
                            return X == Other.X && Y == Other.Y;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const Float2& Other) const noexcept
                        {
                            return !(*this == Other);
                        }
                    };

                    static_assert(sizeof(Float2) == sizeof(KernelTypes::Float) * 2);
                    static_assert(KernelCompiler::IsTriviallyConstructible<Float2>);
                    static_assert(KernelCompiler::IsTriviallyCopyable<Float2>);
                }
            }
        }
    }
}
