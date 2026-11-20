#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"

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

                    [[nodiscard]] inline KernelTypes::Float RayTriangle(
                        const Vector3& Origin, const Vector3& Direction,
                        const Vector3& Vertex0, const Vector3& Vertex1, const Vector3& Vertex2) noexcept
                    {
                        constexpr KernelTypes::Float Epsilon = 1.0e-12f;
                        constexpr KernelTypes::Float MaximumFloat =
                            KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        const Vector3 Edge1 = Vertex1 - Vertex0;
                        const Vector3 Edge2 = Vertex2 - Vertex0;
                        const Vector3 P = Direction.CrossPrecise(Edge2);
                        const KernelTypes::Float Determinant = Edge1.Dot(P);
                        if (MathScalar::Abs(Determinant) < Epsilon)
                            return MaximumFloat;

                        const Vector3 S = Origin - Vertex0;
                        const KernelTypes::Float U = S.Dot(P) / Determinant;
                        const Vector3 Q = S.CrossPrecise(Edge1);
                        const KernelTypes::Float V = Direction.Dot(Q) / Determinant;
                        const KernelTypes::Float T = Edge2.Dot(Q) / Determinant;
                        if (U < 0.0f || V < 0.0f || U + V > 1.0f || T < 0.0f)
                            return MaximumFloat;
                        return T;
                    }

                    [[nodiscard]] inline Vector4 RayTriangle4(const Vector3& Origin, const Vector3& Direction,
                        const Vector4& Vertex0X, const Vector4& Vertex0Y, const Vector4& Vertex0Z,
                        const Vector4& Vertex1X, const Vector4& Vertex1Y, const Vector4& Vertex1Z,
                        const Vector4& Vertex2X, const Vector4& Vertex2Y, const Vector4& Vertex2Z) noexcept
                    {
                        KernelContainers::StaticArray<KernelTypes::Float, 4> Fractions;
                        for (KernelTypes::UInt32 Lane = 0; Lane < 4; ++Lane)
                        {
                            Fractions.EmplaceBack(RayTriangle(Origin, Direction,
                                Vector3(Vertex0X[Lane], Vertex0Y[Lane], Vertex0Z[Lane]),
                                Vector3(Vertex1X[Lane], Vertex1Y[Lane], Vertex1Z[Lane]),
                                Vector3(Vertex2X[Lane], Vertex2Y[Lane], Vertex2Z[Lane])));
                        }
                        return Vector4(Fractions[0], Fractions[1], Fractions[2], Fractions[3]);
                    }
                }
            }
        }
    }
}
