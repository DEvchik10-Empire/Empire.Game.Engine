#include "Vector4.hpp"

#include <cmath>

namespace Math
{
    f32 Vector4::Length() const noexcept
    {
        return std::sqrt(LengthSquared());
    }

    f32 Vector4::LengthSquared() const noexcept
    {
        return X * X +
               Y * Y +
               Z * Z +
               W * W;
    }

    Vector4 Vector4::Normalized() const noexcept
    {
        const f32 Length = this->Length();

        if (Length == 0.0f)
            return {};

        return *this / Length;
    }

    f32 Vector4::Dot(const Vector4& Other) const noexcept
    {
        return X * Other.X +
               Y * Other.Y +
               Z * Other.Z +
               W * Other.W;
    }

    f32 Vector4::Distance(const Vector4& Other) const noexcept
    {
        return (*this - Other).Length();
    }

    f32 Vector4::DistanceSquared(const Vector4& Other) const noexcept
    {
        return (*this - Other).LengthSquared();
    }
}