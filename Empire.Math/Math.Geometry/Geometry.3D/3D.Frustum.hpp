#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Math/Math.Geometry/Geometry.3D/3D.Plane.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Math/Math.Matrix/Matrix.Matrix4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathGeometry
            {
                namespace Geometry3D
                {
                    using namespace EmpireKernel;
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

                        static Frustum FromMatrix(const Matrix4& ViewProjection) noexcept;

                        KernelTypes::Bool ContainsPoint(const Vector3& Point) const noexcept;
                    };
                }
            }
        }
    }
}
