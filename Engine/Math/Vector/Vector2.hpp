#pragma once

#include "Aliases.hpp"

using namespace Engine;

namespace Math
{
    struct Vector2
    {
        f32 X;
        f32 Y;

        constexpr Vector2() noexcept
            : X(0.0f), Y(0.0f)
        {
        }

        constexpr Vector2(f32 X, f32 Y) noexcept
            : X(X), Y(Y)
        {
        }

        [[nodiscard]]
        f32 Length() const noexcept;

        [[nodiscard]]
        f32 LengthSquared() const noexcept;

        [[nodiscard]]
        Vector2 Normalized() const noexcept;

        [[nodiscard]]
        f32 Dot(const Vector2& Other) const noexcept;

        [[nodiscard]]
        f32 Distance(const Vector2& Other) const noexcept;

        [[nodiscard]]
        f32 DistanceSquared(const Vector2& Other) const noexcept;

        constexpr Vector2 operator+(const Vector2& Other) const noexcept
        {
            return { X + Other.X, Y + Other.Y };
        }

        constexpr Vector2 operator-(const Vector2& Other) const noexcept
        {
            return { X - Other.X, Y - Other.Y };
        }

        constexpr Vector2 operator*(f32 Scalar) const noexcept
        {
            return { X * Scalar, Y * Scalar };
        }

        constexpr Vector2 operator/(f32 Scalar) const noexcept
        {
            return { X / Scalar, Y / Scalar };
        }

        constexpr Vector2 operator-() const noexcept
        {
            return { -X, -Y };
        }

        constexpr Vector2& operator+=(const Vector2& Other) noexcept
        {
            X += Other.X;
            Y += Other.Y;

            return *this;
        }

        constexpr Vector2& operator-=(const Vector2& Other) noexcept
        {
            X -= Other.X;
            Y -= Other.Y;

            return *this;
        }

        constexpr Vector2& operator*=(f32 Scalar) noexcept
        {
            X *= Scalar;
            Y *= Scalar;

            return *this;
        }

        constexpr Vector2& operator/=(f32 Scalar) noexcept
        {
            X /= Scalar;
            Y /= Scalar;

            return *this;
        }

        constexpr b8 operator==(const Vector2& Other) const noexcept
        {
            return X == Other.X && Y == Other.Y;
        }

        constexpr b8 operator!=(const Vector2& Other) const noexcept
        {
            return !(*this == Other);
        }
    };

    constexpr Vector2 operator*(f32 Scalar, const Vector2& Vector) noexcept
    {
        return Vector * Scalar;
    }
}