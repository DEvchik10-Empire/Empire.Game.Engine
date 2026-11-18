#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Plane.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathGeometry
                {
                    namespace Geometry3D
                    {
                        using namespace EngineKernel;
                        using MathMatrix::DepthRange;
                    using MathMatrix::Matrix4;
                        using MathVector::Vector3;

                        struct Frustum
                        {
                            Plane Near;
                            Plane Far;
                            Plane Left;
                            Plane Right;
                            Plane Top;
                            Plane Bottom;

                            static Frustum FromMatrix(const Matrix4& ViewProjection, DepthRange Depth = DepthRange::ZeroToOne) noexcept;

                            KernelTypes::Bool ContainsPoint(const Vector3& Point) const noexcept;
                        };
                    }
                }
            }
        }
    }
}
