#include "Vector2.hpp"

#include <cmath>

namespace Math
{
    f32 Vector2::Length() const noexcept
    {
        return std::sqrt(LengthSquared());
    }

    f32 Vector2::LengthSquared() const noexcept
    {
        return X * X + Y * Y;
    }

    Vector2 Vector2::Normalized() const noexcept
    {
        const f32 Length = this->Length();

        if (Length == 0.0f)
            return {};

        return *this / Length;
    }

    f32 Vector2::Dot(const Vector2& Other) const noexcept
    {
        return X * Other.X + Y * Other.Y;
    }

    f32 Vector2::Distance(const Vector2& Other) const noexcept
    {
        return (*this - Other).Length();
    }

    f32 Vector2::DistanceSquared(const Vector2& Other) const noexcept
    {
        return (*this - Other).LengthSquared();
    }
}