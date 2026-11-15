#include "Empire.Math/Math.Geometry/Geometry.3D/3D.Frustum.hpp"

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

                    namespace
                    {
                        // Плоскость из коэффициентов A*x + B*y + C*z + D >= 0 (внутренняя сторона).
                        // Plane хранит Normal и Distance как Normal.Dot(P) - Distance, поэтому Distance = -D.
                        // Коэффициенты нормализуются вместе с нормалью.
                        Plane MakePlane(KernelTypes::Float A, KernelTypes::Float B, KernelTypes::Float C, KernelTypes::Float D) noexcept
                        {
                            const Vector3 Normal(A, B, C);
                            const KernelTypes::Float Length = Normal.Length();

                            if (Length <= 0.0f)
                                return Plane();

                            return Plane(Normal / Length, -D / Length);
                        }
                    }

                    // Матрица строчная, вектор столбец (clip = M * v): плоскости получаются из строк матрицы.
                    Frustum Frustum::FromMatrix(const Matrix4& ViewProjection, DepthRange Depth) noexcept
                    {
                        const Matrix4& M = ViewProjection;
                        Frustum Result;

                        Result.Left   = MakePlane(M.M30 + M.M00, M.M31 + M.M01, M.M32 + M.M02, M.M33 + M.M03);
                        Result.Right  = MakePlane(M.M30 - M.M00, M.M31 - M.M01, M.M32 - M.M02, M.M33 - M.M03);
                        Result.Bottom = MakePlane(M.M30 + M.M10, M.M31 + M.M11, M.M32 + M.M12, M.M33 + M.M13);
                        Result.Top    = MakePlane(M.M30 - M.M10, M.M31 - M.M11, M.M32 - M.M12, M.M33 - M.M13);
                        // Глубина -1..1: z >= -w, глубина 0..1: z >= 0
        if (Depth == DepthRange::ZeroToOne)
            Result.Near = MakePlane(M.M20, M.M21, M.M22, M.M23);
        else
            Result.Near = MakePlane(M.M30 + M.M20, M.M31 + M.M21, M.M32 + M.M22, M.M33 + M.M23);

                        Result.Far    = MakePlane(M.M30 - M.M20, M.M31 - M.M21, M.M32 - M.M22, M.M33 - M.M23);

                        return Result;
                    }

                    KernelTypes::Bool Frustum::ContainsPoint(const Vector3& Point) const noexcept
                    {
                        if (Near.DistanceToPoint(Point) < 0.0f)
                            return false;

                        if (Far.DistanceToPoint(Point) < 0.0f)
                            return false;

                        if (Left.DistanceToPoint(Point) < 0.0f)
                            return false;

                        if (Right.DistanceToPoint(Point) < 0.0f)
                            return false;

                        if (Top.DistanceToPoint(Point) < 0.0f)
                            return false;

                        if (Bottom.DistanceToPoint(Point) < 0.0f)
                            return false;

                        return true;
                    }
                }
            }
        }
    }
}
