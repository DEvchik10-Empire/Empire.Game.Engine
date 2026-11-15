#include "Frustum.hpp"

namespace Math
{
    Frustum Frustum::FromMatrix(const Matrix4& ViewProjection) noexcept
    {
        Frustum Result;

        // Left
        Result.Left = Plane(
            Vector3(
                ViewProjection.M03 + ViewProjection.M00,
                ViewProjection.M13 + ViewProjection.M10,
                ViewProjection.M23 + ViewProjection.M20
            ),
            ViewProjection.M33 + ViewProjection.M30
        );

        // Right
        Result.Right = Plane(
            Vector3(
                ViewProjection.M03 - ViewProjection.M00,
                ViewProjection.M13 - ViewProjection.M10,
                ViewProjection.M23 - ViewProjection.M20
            ),
            ViewProjection.M33 - ViewProjection.M30
        );

        // Bottom
        Result.Bottom = Plane(
            Vector3(
                ViewProjection.M03 + ViewProjection.M01,
                ViewProjection.M13 + ViewProjection.M11,
                ViewProjection.M23 + ViewProjection.M21
            ),
            ViewProjection.M33 + ViewProjection.M31
        );

        // Top
        Result.Top = Plane(
            Vector3(
                ViewProjection.M03 - ViewProjection.M01,
                ViewProjection.M13 - ViewProjection.M11,
                ViewProjection.M23 - ViewProjection.M21
            ),
            ViewProjection.M33 - ViewProjection.M31
        );

        // Near
        Result.Near = Plane(
            Vector3(
                ViewProjection.M03 + ViewProjection.M02,
                ViewProjection.M13 + ViewProjection.M12,
                ViewProjection.M23 + ViewProjection.M22
            ),
            ViewProjection.M33 + ViewProjection.M32
        );

        // Far
        Result.Far = Plane(
            Vector3(
                ViewProjection.M03 - ViewProjection.M02,
                ViewProjection.M13 - ViewProjection.M12,
                ViewProjection.M23 - ViewProjection.M22
            ),
            ViewProjection.M33 - ViewProjection.M32
        );

        return Result;
    }

    b8 Frustum::ContainsPoint(const Vector3& Point) const noexcept
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