#include "AABB.hpp"

namespace Math
{
    Vector3 AABB::Center() const noexcept
    {
        return (Min + Max) / 2.0f;
    }

    Vector3 AABB::Extents() const noexcept
    {
        return (Max - Min) / 2.0f;
    }

    Vector3 AABB::Size() const noexcept
    {
        return Max - Min;
    }

    b8 AABB::Contains(const Vector3& Point) const noexcept
    {
        return
        Point.X >= Min.X && Point.X <= Max.X &&
        Point.Y >= Min.Y && Point.Y <= Max.Y &&
        Point.Z >= Min.Z && Point.Z <= Max.Z;
    }

    b8 AABB::Intersects(const AABB& Other) const noexcept
    {
        return !(
            Max.X < Other.Min.X || Other.Max.X < Min.X ||
            Max.Y < Other.Min.Y || Other.Max.Y < Min.Y ||
            Max.Z < Other.Min.Z || Other.Max.Z < Min.Z
        );
    }

    /*AABB AABB::Union(const AABB& Other) const noexcept
    {
        AABB Result;

        Result.Min.X = Min.X < Other.Min.X ? Min.X : Other.Min.X;
        Result.Min.Y = Min.Y < Other.Min.Y ? Min.Y : Other.Min.Y;
        Result.Min.Z = Min.Z < Other.Min.Z ? Min.Z : Other.Min.Z;

        Result.Max.X = Max.X > Other.Max.X ? Max.X : Other.Max.X;
        Result.Max.Y = Max.Y > Other.Max.Y ? Max.Y : Other.Max.Y;
        Result.Max.Z = Max.Z > Other.Max.Z ? Max.Z : Other.Max.Z;

        return Result;

    } */

    AABB AABB::Union(const AABB& Other) const noexcept
    {
        AABB Result;

        Result.Min.X = Min.X;
        if (Other.Min.X < Result.Min.X)
            Result.Min.X = Other.Min.X;

        Result.Min.Y = Min.Y;
        if (Other.Min.Y < Result.Min.Y)
            Result.Min.Y = Other.Min.Y;

        Result.Min.Z = Min.Z;
        if (Other.Min.Z < Result.Min.Z)
            Result.Min.Z = Other.Min.Z;

        Result.Max.X = Max.X;
        if (Other.Max.X > Result.Max.X)
            Result.Max.X = Other.Max.X;

        Result.Max.Y = Max.Y;
        if (Other.Max.Y > Result.Max.Y)
            Result.Max.Y = Other.Max.Y;

        Result.Max.Z = Max.Z;
        if (Other.Max.Z > Result.Max.Z)
            Result.Max.Z = Other.Max.Z;

        return Result;
    }
}