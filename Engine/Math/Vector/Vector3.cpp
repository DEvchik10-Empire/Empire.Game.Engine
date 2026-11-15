#include "Vector3.hpp"

#include <cmath>

namespace Math
{
    f32 Vector3::Length() const noexcept
    {
        return std::sqrt(LengthSquared());
    }

    f32 Vector3::LengthSquared() const noexcept
    {
        return X * X + Y * Y + Z * Z;
    }

    Vector3 Vector3::Normalized() const noexcept
    {
        const f32 Length = this->Length();

        if (Length == 0.0f)
            return {};

        return *this / Length;
    }

    f32 Vector3::Dot(const Vector3& Other) const noexcept
    {
        return X * Other.X +
               Y * Other.Y +
               Z * Other.Z;
    }

    Vector3 Vector3::Cross(const Vector3& Other) const noexcept
    {
        return {
            Y * Other.Z - Z * Other.Y,
            Z * Other.X - X * Other.Z,
            X * Other.Y - Y * Other.X
        };
    }

    f32 Vector3::Distance(const Vector3& Other) const noexcept
    {
        return (*this - Other).Length();
    }

    f32 Vector3::DistanceSquared(const Vector3& Other) const noexcept
    {
        return (*this - Other).LengthSquared();
    }
}