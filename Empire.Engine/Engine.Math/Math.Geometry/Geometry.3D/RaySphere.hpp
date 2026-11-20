#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/FindRoot.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace Geometry3D
                {
                    using namespace EngineKernel;
                    using namespace MathVector;

                    [[nodiscard]] inline KernelTypes::Float RaySphere(
                        const Vector3& Origin, const Vector3& Direction,
                        const Vector3& Center, KernelTypes::Float Radius) noexcept
                    {
                        const Vector3 CenterToOrigin = Origin - Center;
                        const KernelTypes::Float A = Direction.LengthSq();
                        const KernelTypes::Float B = 2.0f * Direction.Dot(CenterToOrigin);
                        const KernelTypes::Float C = CenterToOrigin.LengthSq() - Radius * Radius;
                        KernelTypes::Float Fraction1 = 0.0f;
                        KernelTypes::Float Fraction2 = 0.0f;
                        const KernelTypes::Int32 RootCount =
                            MathScalar::FindRoot(A, B, C, Fraction1, Fraction2);
                        if (RootCount == 0)
                            return C <= 0.0f ? 0.0f : KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        if (Fraction1 > Fraction2)
                        {
                            const KernelTypes::Float Temporary = Fraction1;
                            Fraction1 = Fraction2;
                            Fraction2 = Temporary;
                        }
                        if (Fraction1 >= 0.0f)
                            return Fraction1;
                        return Fraction2 >= 0.0f ? 0.0f : KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                    }

                    [[nodiscard]] inline KernelTypes::Int32 RaySphere(
                        const Vector3& Origin, const Vector3& Direction, const Vector3& Center,
                        KernelTypes::Float Radius, KernelTypes::Float& OutMinimum,
                        KernelTypes::Float& OutMaximum) noexcept
                    {
                        const Vector3 CenterToOrigin = Origin - Center;
                        const KernelTypes::Float A = Direction.LengthSq();
                        const KernelTypes::Float B = 2.0f * Direction.Dot(CenterToOrigin);
                        const KernelTypes::Float C = CenterToOrigin.LengthSq() - Radius * Radius;
                        const KernelTypes::Int32 RootCount =
                            MathScalar::FindRoot(A, B, C, OutMinimum, OutMaximum);
                        if (RootCount == 0 && C <= 0.0f)
                        {
                            OutMinimum = OutMaximum = 0.0f;
                            return 1;
                        }
                        if (RootCount <= 1)
                            return RootCount;
                        if (OutMinimum > OutMaximum)
                        {
                            const KernelTypes::Float Temporary = OutMinimum;
                            OutMinimum = OutMaximum;
                            OutMaximum = Temporary;
                        }
                        return 2;
                    }
                }
            }
        }
    }
}
